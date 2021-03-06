//-----------------------------------------------------------------------------
//
// ComponentEmitter.js
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

ComponentEmitter = class.extendStatic(Component);

class.properties(ComponentEmitter,
{
    //------------------------------------------------------------------------
    // VARS
    //------------------------------------------------------------------------
    
    fx : "fx/ambience_tall_fire2.kfx",
    
    //------------------------------------------------------------------------
    // FUNCTIONS
    //------------------------------------------------------------------------
    
    //------------------------------------------------------------------------
    // EVENTS
    //------------------------------------------------------------------------
    
    onTrigger : function(instigator, args)
    {
    },
    
    onReady : function()
    {
        this.parent.owner.setBounds(-1024, -1024, -1024, 1024, 1024, 1024);
    },
    
    onLocalTick : function()
    {
        var actor = this.parent.owner;
        var org = actor.origin;
        
        actor.spawnFX(this.fx, 0, 0, 1.024);
    }
});
