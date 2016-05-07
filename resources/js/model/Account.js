var AccountBase = Backbone.Model.extend({

    getAddress:function(){
        return this.get("address")||this.get("stealth");
    },

    backup:function(path){
        return XETH_wallet.exportKey({address:this.getAddress(), path:path});
    },

    changePassword:function(previous, password){
        return XETH_wallet.changePassword({password: previous, newPassword:password, address:this.getAddress()});
    }
});

var Account = AccountBase.extend({

    idAttribute: "address",

    initialize:function(){
        _(this).bindAll("update", "removeIfEmpty", "destroy");
        this.update();
        if(this.get("stealth"))
        {
            this.on("change:balance", this.removeIfEmpty);
        }
        this.pinned = 0;
        this.timer = undefined;
//        this.autoUpdate();
    },

    removeIfEmpty: function(){
        if(!this.pinned)
        {
            if((this.get("balance")==0)&&(this.get("unconfirmed")==0))
            {
                this.trigger("removing");
                setTimeout(this.destroy, 30000);
            }
        }
    },

    pin:function(){
        this.pinned++;
    },

    unpin:function(){
        this.pinned--;
        if(this.pinned <= 0)
        {
            this.pinned = 0;
            if(this.get("stealth")) this.removeIfEmpty();
        }
    },

    update:function(){
        try
        {
            var address = this.get("address");
            this.set("balance", XETH_convert.fromWei(XETH_wallet.getBalance(address)));
            this.set("unconfirmed", XETH_convert.fromWei(XETH_wallet.getPendingBalance(address)));
        }
        catch(e)
        {}
    },

    updateAsync:function(callback){
        var address = this.get("address");
        var status = {value: 0};
        this.watchBalanceFuture(new FutureObserver(XETH_wallet.getBalanceAsync(address)), status, "balance", callback);
        this.watchBalanceFuture(new FutureObserver(XETH_wallet.getPendingBalanceAsync(address)), status, "unconfirmed", callback);
    },

    watchBalanceFuture:function(observer, status, type, callback){
        var self = this;
        observer.onFinished(function(){
            var balance = observer.future.getResult();
            self.set(type, XETH_convert.fromWei(balance));
            status.value++;
            if(status.value > 1 && callback instanceof Function) callback();
            observer.future.dispose();
        });
    },

    autoUpdate:function(){
        var self = this;
        this.timer  = setTimeout(function(){
            self.updateAsync(function(){
                self.autoUpdate();
            });
        },5000);
    },

    send:function(request, callback){
        request.from = this.get("address");
        if(!isNaN(request.amount))
        {
            request.amount = XETH_convert.toWei(""+request.amount);
        }
        
        if(callback){
            var observer = new FutureObserver(this.sendAsync(request));
            var self = this;
            observer.onFinished(function(result){
                if(result) self.update();
                callback(result);
                observer.future.dispose();
            });
        }else{
            var txid = XETH_wallet.send(request);
            if(txid){
                this.update();
            }
            return txid;
        }
    },

    destroy: function(){
        if(this.interval) clearTimeout(this.interval);
        this.trigger("destroy", this);
    }

});


var StealthAccount = AccountBase.extend({

    idAttribute: "stealth",

    initialize:function(){
        this.set("balance",0);
        this.set("unconfirmed", 0);
    },
    pin:function(){},
    unpin:function(){},
    update:function(){},
    autoUpdate:function(){},
    send:function(){
        return false;
    }
});


var AccountCollection = Backbone.Collection.extend({

    initialize:function(models, options){
        _(this).bindAll("add", "parseNew", "linkProfile");
        this.profiles = options.profiles;
    },

    add:function(data){
        var account = this.get(data.address||data.stealth);
        if(!account)
        {
            Backbone.Collection.prototype.add.call(this, data);
        }
    },

    observe: function(){
        XETH_event.Account.connect(this, this.parseNew);
    },

    parseNew: function(data){
        var model = this.model(data);
        var profile = this.profiles.find({account:model.get("address")});
        if(model.get("balance") != 0 || model.get("unconfirmed") != 0 || !model.get("stealth") || !model.get("address") || profile)
        {
            if(profile) model.set("profile", profile);
            model.autoUpdate();
            this.add(model);
        }
    },

    fetch:function(){
        if(!this.profiles.length) this.profiles.fetch();
        var accounts = XETH_wallet.getAccounts();
        this.reset(this.filterData(accounts));
        this.profiles.on("add", this.linkProfile);
    },

    filterData:function(accounts){
        var result = [];
        for(var i in accounts)
        {
            var model = this.model(accounts[i]);
            var profile = this.profiles.find({account:model.get("address")});
            if(profile) model.set("profile", profile);
            if(!profile && model.get("stealth") && model.get("address"))
            {
                if(model.get("balance")!=0 || model.get("unconfirmed")!=0)
                {
                    model.autoUpdate();
                    result.push(model);
                }
            }
            else
            {
                model.autoUpdate();
                result.push(model);
            }
        }
        return result;
    },

    linkProfile:function(profile){
        var account = this.find({address:profile.get("account")});
        if(account)
        {
            account.set("profile", profile);
        }
    },

    generate:function(request){
        return XETH_wallet.generateKey(request);
    },

    importKey:function(file, password){
        var request = {file:file, password:password};
        return XETH_wallet.importKey(request);
    },

    modelId: function(attrs){
        return attrs.address;
    },

    model: function(attrs, options) {
        var account;
        if(attrs.address==undefined){
            account = new StealthAccount(attrs, options);
        }
        else{
            account = new Account(attrs, options);
        }
        return account;
    },

    sync:function(){}

});
