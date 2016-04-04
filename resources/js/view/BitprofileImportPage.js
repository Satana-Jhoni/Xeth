var BitprofileImportPageView = SubPageView.extend({

    initialize:function(options){
        _(this).bindAll("open", "render", "submit");
		SubPageView.prototype.initialize.call(this,options);
        this.template = options.templates.get("import_bitprofile");
        this.filesystem = options.filesystem;
        this.profiles = options.profiles;
        this.router = options.router;
    },
    
    render:function(){
        this.$el.html(this.template());
        this.$el.find(".btn.browse").click(this.render);
        this.$el.find(".btnSubmit").click(this.submit);
        this.fileInput = this.$el.find("#importBitprofile_address");
    },

    open:function(){
        this.filename = this.filesystem.browse({type:"open"});
        this.fileInput.val(this.filename||"");
    },

    submit:function(){
        if(!this.filename){
            notifyError("please select a file");
            return false;
        }
        if(!this.profiles.importKey(this.filename)){
            notifyError("failed to import key, file is corrupted");
            return false;
        }
        notifySuccess("bitprofile imported");
        //this.router.redirect("bitprofile",{subpage:"view"});
        return true;
    }

});
