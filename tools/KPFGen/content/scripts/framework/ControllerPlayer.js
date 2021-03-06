//-----------------------------------------------------------------------------
//
// ControllerPlayer.js
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

const STATE_MOVE_NONE   = 0;
const STATE_MOVE_WALK   = 1;
const STATE_MOVE_AIR    = 2;
const STATE_MOVE_CLIMB  = 3;
const STATE_MOVE_SWIM   = 4;
const STATE_MOVE_NOCLIP = 5;

ControllerPlayer = class.extends(Controller, function()
{
    //------------------------------------------------------------------------
    // CONSTANTS
    //------------------------------------------------------------------------
    
    const CRAWL_FLOORHEIGHT         = 15.36;
    
    const ANGLE_MAXPITCH            = Angle.degToRad(90);
    
    const JUMP_VELOCITY             = 608;
    const JUMP_GROUNDEPISILON       = 0.512;
    const JUMP_SLOPEHEIGHTMIN       = 8;
    
    const WALK_STRAFEROLL_ANGLE     = 0.0835;
    const WALK_STRAFEROLL_SPEED     = 0.1;

    const SLOPESLIDE_DIST           = 10.24;
    const SLOPESLIDE_VELOCITY       = 8.0;
    const SLOPESLIDE_LERP           = 0.5;

    const MOVE_FRICTION             = 1.0;
    const MOVE_GRAVITY              = 1843.2;

    const WALKSPEED_F_FORWARD       = 384;
    const WALKSPEED_F_BACKWARD      = -292;
    const WALKSPEED_F_ACCEL         = 0.09;
    const WALKSPEED_F_DEAACCEL      = 0.5;

    const WALKSPEED_R_FORWARD       = 384;
    const WALKSPEED_R_BACKWARD      = -384;
    const WALKSPEED_R_ACCEL         = 0.25;
    const WALKSPEED_R_DEAACCEL      = 0.5;
    
    const AIRSPEED_F_FORWARD        = 384;
    const AIRSPEED_F_BACKWARD       = -292;
    const AIRSPEED_F_ACCEL          = 0.05;
    const AIRSPEED_F_DEAACCEL       = 0.25;

    const AIRSPEED_R_FORWARD        = 384;
    const AIRSPEED_R_BACKWARD       = -384;
    const AIRSPEED_R_ACCEL          = 0.05;
    const AIRSPEED_R_DEAACCEL       = 0.25;
    
    const SWIMROLL_MAXANGLE         = 0.33;
    const SWIMROLL_SPEED            = 0.05;

    const SWIM_FORWARDSPEED         = 245.76;
    const SWIM_BACKSPEED            = 153.5;
    const SWIM_SIDESPEED            = 245.76;
    const SWIM_UPSPEED              = 338;
    const SWIM_ACCELERATION         = 0.01953125;
    const SWIM_DEACCELERATION       = 0.05;
    const SWIM_UPDEACCELERATION     = 0.111;
    const SWIM_THRUSTSPEED          = 480;
    const SWIM_THRUSTDEACCELERATE   = 0.02;

    // TODO: the original game specifies '6.2832' but time is calculated differently here
    const SWIM_MOVETIME             = 512;
    
    const CLIMBSPEED_F_FORWARD      = 215;
    const CLIMBSPEED_F_BACKWARD     = 0;
    const CLIMBSPEED_F_ACCEL        = 0.0625;
    const CLIMBSPEED_F_DEAACCEL     = 0.5;
    
    const CLIMB_DIST                = 1;
    const CLIMB_FACEANGLE           = 40.0;
    const CLIMB_YAWLERP             = 0.084;
    const CLIMB_BOBANGLE            = 0.33;
    const CLIMB_BOBSPEED            = 0.025;
    const CLIMB_BOBWINDOWN          = 0.035;
    const CLIMB_LEAP_AMNT           = 353.28;
    const CLIMB_RANGE               = 4.096;
    const CLIMB_LERP_SPEED          = 40.96;

    // TODO: the original game specifies '11' but time is calculated differently here
    const CLIMB_MOVETIME            = 687;
    
    const FALL_THRESHOLD_THUMP      = 8.192;
    const FALL_THRESHOLD_STOPSPEED  = 7.68;
    const FALL_THUMPFACTOR          = 16;
    
    //------------------------------------------------------------------------
    // VARS
    //------------------------------------------------------------------------
    
    this.state                      = STATE_MOVE_NONE;          // current move state
    this.mouse_x                    = 0.0;
    this.mouse_y                    = 0.0;
    this.bCrawling                  = false;                    // moving at half the view height and speed
    this.bNoClip                    = false;                    // free movement/clipping
    this.bFlying                    = false;
    
    this.waterheight                = 0;                        // the Y-plane height of a water plane
    this.waterlevel                 = WL_INVALID;               // used to determine swim movement behavior
    this.bLerping                   = false;                    // pulled back to surface?
    this.bRollDir                   = true;                     // view roll bobbing for climbing
    
    //------------------------------------------------------------------------
    // OBJECTS
    //------------------------------------------------------------------------
    
    this.speed = [
    // STATE_MOVE_NONE
    {
        forward :
        {
            forwardspeed    : 0,
            backspeed       : 0,
            acceleration    : 0,
            deacceleration  : 0
        },
        
        right :
        {
            forwardspeed    : 0,
            backspeed       : 0,
            acceleration    : 0,
            deacceleration  : 0
        }
    },
    // STATE_MOVE_WALK
    {
        forward :
        {
            forwardspeed    : WALKSPEED_F_FORWARD,
            backspeed       : WALKSPEED_F_BACKWARD,
            acceleration    : WALKSPEED_F_ACCEL,
            deacceleration  : WALKSPEED_F_DEAACCEL
        },
        
        right :
        {
            forwardspeed    : WALKSPEED_R_FORWARD,
            backspeed       : WALKSPEED_R_BACKWARD,
            acceleration    : WALKSPEED_R_ACCEL,
            deacceleration  : WALKSPEED_R_DEAACCEL
        }
    },
    // STATE_MOVE_AIR
    {
        forward :
        {
            forwardspeed    : AIRSPEED_F_FORWARD,
            backspeed       : AIRSPEED_F_BACKWARD,
            acceleration    : AIRSPEED_F_ACCEL,
            deacceleration  : AIRSPEED_F_DEAACCEL
        },
        
        right :
        {
            forwardspeed    : AIRSPEED_R_FORWARD,
            backspeed       : AIRSPEED_R_BACKWARD,
            acceleration    : AIRSPEED_R_ACCEL,
            deacceleration  : AIRSPEED_R_DEAACCEL
        }
    },
    // STATE_MOVE_CLIMB
    {
        forward :
        {
            forwardspeed    : CLIMBSPEED_F_FORWARD,
            backspeed       : CLIMBSPEED_F_BACKWARD,
            acceleration    : CLIMBSPEED_F_ACCEL,
            deacceleration  : CLIMBSPEED_F_DEAACCEL
        },
        
        right :
        {
            forwardspeed    : 0,
            backspeed       : 0,
            acceleration    : 0,
            deacceleration  : 0
        }
    },
    // STATE_MOVE_SWIM
    {
        forward :
        {
            forwardspeed    : SWIM_FORWARDSPEED,
            backspeed       : -SWIM_BACKSPEED,
            acceleration    : SWIM_ACCELERATION,
            deacceleration  : SWIM_DEACCELERATION
        },
        
        right :
        {
            forwardspeed    : SWIM_SIDESPEED,
            backspeed       : -SWIM_SIDESPEED,
            acceleration    : SWIM_ACCELERATION,
            deacceleration  : SWIM_DEACCELERATION
        }
    },
    // STATE_MOVE_NOCLIP
    {
        forward :
        {
            forwardspeed    : 1024,
            backspeed       : -1024,
            acceleration    : 0.1,
            deacceleration  : 0.5
        },
        
        right :
        {
            forwardspeed    : 1024,
            backspeed       : -1024,
            acceleration    : 0.1,
            deacceleration  : 0.5
        }
    }];
    
    //------------------------------------------------------------------------
    // FUNCTIONS
    //------------------------------------------------------------------------
    
    this.accelerate = function(properties)
    {
        var fwd = properties.forward;
        var rgt = properties.right;
        
        // accel Z axis
        if(this.command.getAction('+forward'))
            this.accelZ(fwd.forwardspeed, fwd.acceleration);
        else if(this.command.getAction('+back'))
            this.accelZ(fwd.backspeed, fwd.acceleration);
        else
            this.deAccelZ(fwd.deacceleration);
        
        // accel X axis
        if(this.command.getAction('+strafeleft'))
            this.accelX(rgt.forwardspeed, rgt.acceleration);
        else if(this.command.getAction('+straferight'))
            this.accelX(rgt.backspeed, rgt.acceleration);
        else
            this.deAccelX(rgt.deacceleration);
        
        // apply acceleration to velocity
        this.velocity.add(Vector.gScale(this.forward, this.accel.z));
        this.velocity.add(Vector.gScale(this.right, this.accel.x));
    }
    
    // play 'oof' sound and thump the view height if the player
    // hit the floor hard enough
    this.checkFallLand = function(oldVelocity)
    {
        var y = (oldVelocity - this.velocity.y) * this.frametime;
        
        if(this.origin.y - this.plane.distance(this.origin) <= 0.1024 &&
            y <= -FALL_THUMPFACTOR)
        {
            // TODO - determine if client or server
            if(this.local == true)
                Snd.play('sounds/shaders/generic_22_turok_land.ksnd');
        }
        
        if(y <= -FALL_THRESHOLD_THUMP)
        {
            if(y <= -FALL_THRESHOLD_STOPSPEED)
                this.accel.clear();
            
            if(this.local == true)
                ClientPlayer.camera.viewHeight =
                    (this.owner.viewHeight * 0.5);
        }
    }
    
    //------------------------------------------------------------------------
    // JUMPING
    //------------------------------------------------------------------------
    
    this.jump = function()
    {
        if(this.bCrawling)
            return;
        
        this.velocity.y = JUMP_VELOCITY;
        
        // TODO - determine if client or server
        if(this.local == true)
            Snd.play('sounds/shaders/generic_21_turok_jump.ksnd');
    }
    
    // basic jumping routine checks
    this.canJump = function()
    {
        var plane = this.plane;
        var origin = this.origin;
        
        if(this.state == STATE_MOVE_AIR)
        {
            if(origin.y - plane.distance(origin) > JUMP_GROUNDEPISILON)
                return true;
            else
                return false;
        }
        
        if(this.command.getAction('+jump') && this.command.getActionHeldTime('+jump') == 0)
        {
            var velocity = this.velocity;
            
            // can't jump if standing on a steep slope
            if(plane.isAWall() && origin.y - plane.distance(origin) <= JUMP_SLOPEHEIGHTMIN)
                return false;
            
            // must be standing on ground or walking off a ledge
            if(origin.y - plane.distance(origin) <= JUMP_GROUNDEPISILON ||
                (velocity.y < 0 && velocity.y > -JUMP_VELOCITY))
            {
                this.jump();
                return true;
            }
        }
        
        return false;
    }
    
    //------------------------------------------------------------------------
    // MISC MOVEMENT
    //------------------------------------------------------------------------
    
    this.noClipMove = function()
    {
        this.setDirection(
            this.angles.pitch,
            this.angles.yaw,
            0);
            
        this.accelerate(this.speed[STATE_MOVE_NOCLIP]);
        this.deAccelY(0.5);
        this.origin.x += (this.velocity.x * this.frametime);
        this.origin.y += (this.velocity.y * this.frametime);
        this.origin.z += (this.velocity.z * this.frametime);
        this.velocity.clear();
    }
    
    this.flyMove = function()
    {
        this.setDirection(
            this.angles.pitch,
            this.angles.yaw,
            0);
            
        this.accelerate(this.speed[STATE_MOVE_NOCLIP]);
        this.deAccelY(0.5);
        this.super.prototype.beginMovement.bind(this)();
        this.velocity.clear();
    }
    
    //------------------------------------------------------------------------
    // WALKING/RUNNING
    //------------------------------------------------------------------------
    
    this.walkMove = function()
    {
        this.setDirection(
            0,
            this.angles.yaw,
            0);

        if(!this.plane.isAWall() || this.plane.flags & 16)
            this.accelerate(this.speed[STATE_MOVE_WALK]);
        
        // rolls the player's view when strafing
        if(this.command.getAction('+strafeleft'))
        {
            this.angles.roll = Math.lerp(this.angles.roll,
                -WALK_STRAFEROLL_ANGLE, WALK_STRAFEROLL_SPEED);
        }
        else if(this.command.getAction('+straferight'))
        {
            this.angles.roll = Math.lerp(this.angles.roll,
                WALK_STRAFEROLL_ANGLE, WALK_STRAFEROLL_SPEED);
        }
        else
        {
            this.angles.roll = Math.lerp(this.angles.roll,
                0, WALK_STRAFEROLL_SPEED);
                
            if(this.angles.roll <= 0.001 && this.angles.roll >= -0.001)
                this.angles.roll = 0;
        }
        
        var plane = this.plane;
        var origin = this.origin;
        
        // acceleration is slower when crawling
        if(this.bCrawling)
        {
            var t = 80 * this.frametime;
            
            if(t != 0)
            {
                this.accel.x *= (1 / t);
                this.accel.z *= (1 / t);
            }
        }
        
        var y = this.velocity.y;
        
        this.super.prototype.beginMovement.bind(this)();
        
        if(!(plane.flags & 16))
            this.gravity(MOVE_GRAVITY);
        
        if(!plane.isAWall() || plane.flags & 16)
            this.applyFriction(MOVE_FRICTION);
            
        if(plane.flags & 16)
            this.applyVerticalFriction(MOVE_FRICTION);
        
        this.checkFallLand(y);
    }
    
    //------------------------------------------------------------------------
    // AIR MOVEMENT
    //------------------------------------------------------------------------
    
    this.airMove = function()
    {
        this.setDirection(
            0,
            this.angles.yaw,
            0);
            
        this.accelerate(this.speed[STATE_MOVE_AIR]);
        
        var y = this.velocity.y;
        
        this.super.prototype.beginMovement.bind(this)();
        
        this.gravity(MOVE_GRAVITY);
        this.applyFriction(MOVE_FRICTION);
        this.checkFallLand(y);
    }
    
    //------------------------------------------------------------------------
    // CLIMBING
    //------------------------------------------------------------------------
    
    // determines if player is facing the climbable surface
    this.facingPlane = function(plane)
    {
        // don't bother if we're behind it
        if(this.getClimbDistance(plane) < 0)
            return false;
        
        var angle = Angle.diff(Angle.clamp(plane.getYaw()),
            this.angles.yaw + Math.PI);
        
        if(angle < 0)
            angle = -angle;
        
        if(Angle.radToDeg(angle) > CLIMB_FACEANGLE)
            return false;
            
        return true;
    }
    
    this.getClimbDistance = function(plane)
    {
        var n = plane.normal;
        
        return (Vector.dot(this.origin, n) - 
                Vector.dot(plane.pt1, n));
    }
    
    this.findClimbablePlane = function()
    {
        if(this.plane == null)
            return null;
        
        // test the plane that we're standing in
        if(this.plane.area.ComponentAreaClimb && this.plane.isAWall())
            return this.plane;
            
        return null;
    }
    
    this.canClimb = function()
    {  
        var plane = this.findClimbablePlane();
            
        if(plane != null)
        {
            var y = this.origin.y + this.center_y;
            
            if(this.state != STATE_MOVE_CLIMB)
            {
                // too far from the plane? ignore if we're already climbing
                if(y > plane.pt1.y &&
                    y > plane.pt2.y &&
                    y > plane.pt3.y)
                {
                    return false;
                }
                
                var dist = this.getClimbDistance(this.plane);
                
                // must be within a certain range of the plane
                if(dist > CLIMB_RANGE || dist <= -(CLIMB_RANGE*4))
                    return false;
            }
            
            this.velocity.clear();
            return true;
        }
            
        return false;
    }
    
    this.lerpToPlane = function()
    {
        var plane = this.plane;
        var dist = this.getClimbDistance(plane);
        
        this.velocity.x = -(plane.normal.x * dist) * CLIMB_LERP_SPEED;
        this.velocity.z = -(plane.normal.z * dist) * CLIMB_LERP_SPEED;
    }
    
    this.canClimbLeap = function()
    {
        if(this.state != STATE_MOVE_CLIMB)
            return false;
        
        if(this.command.getAction('+jump') && !this.command.getActionHeldTime('+jump'))
        {
            this.jump();
            
            var an = this.angles.yaw;
            
            this.accel.x =  Math.sin(an) * CLIMB_LEAP_AMNT;
            this.accel.z =  Math.cos(an) * CLIMB_LEAP_AMNT;
            this.state = STATE_MOVE_AIR;
            
            return true;
        }
        
        return false;
    }
    
    this.climbMove = function()
    {
        var yaw;
        
        // always forces the player's yaw to face the surface
        yaw = Angle.clamp(Angle.diff(this.angles.yaw + Math.PI,
            this.plane.getYaw()));
            
        this.angles.yaw = -yaw * CLIMB_YAWLERP + this.angles.yaw;
        this.command.angle_x = this.angles.yaw;
        
        // move along plane normal
        this.setDirection(
            0,
            this.plane.normal.toYaw(),
            0);
        
        if(this.command.getAction('+forward') && this.movetime < this.timestamp)
        {
            this.movetime = CLIMB_MOVETIME + this.timestamp;
            this.accel.clear();
            this.velocity.clear();
            
            if(this.bRollDir == true)
            {
                // TODO - determine if client or server
                if(this.local == true)
                    Snd.play('sounds/shaders/generic_23_turok_climb_1.ksnd');
                    
                this.bRollDir = false;
            }
            else
            {
                // TODO - determine if client or server
                if(this.local == true)
                    Snd.play('sounds/shaders/generic_24_turok_climb_2.ksnd');
                    
                this.bRollDir = true;
            }
        }
        
        var speed = this.speed[STATE_MOVE_CLIMB];
        
        if(this.movetime >= this.timestamp)
        {
            this.accelZ(speed.forward.forwardspeed, speed.forward.acceleration);
            this.accelY(speed.forward.forwardspeed, speed.forward.acceleration);
            
            if(this.bRollDir == true)
            {
                this.angles.roll = Math.lerp(this.angles.roll,
                    CLIMB_BOBANGLE, CLIMB_BOBSPEED);
            }
            else
            {
                this.angles.roll = Math.lerp(this.angles.roll,
                    -CLIMB_BOBANGLE, CLIMB_BOBSPEED);
            }
        }
        else
        {
            this.deAccelZ(speed.forward.deacceleration);
            this.deAccelY(speed.forward.deacceleration);
            
            // wind down the view roll
            this.angles.roll = Math.lerp(this.angles.roll, 0, CLIMB_BOBWINDOWN);
        }
        
        this.velocity.add(Vector.gScale(this.forward, this.accel.z));
        this.velocity.y = this.accel.y;
        
        this.lerpToPlane();
        Physics.move(this);
            
        this.applyFriction(MOVE_FRICTION);
        this.velocity.y = 0;
    }
    
    //------------------------------------------------------------------------
    // SWIMMING
    //------------------------------------------------------------------------
    
    this.getSinkHeight = function()
    {
        var p = this.plane;
        
        if(p == null)
            return 0.0;
            
        var origin = this.origin;
        var dist = this.plane.distance(origin);
        
        if(origin.y - dist <= 0.512)
            return 0.0;
            
        return dist;
    }
    
    this.checkWater = function()
    {
        var p = this.plane;
        
        if(p != null && p.area.ComponentAreaWater)
            return p.area.ComponentAreaWater.getWaterLevel(this);

        return WL_INVALID;
    }
    
    this.inWater = function()
    {
        var oldLevel = this.waterlevel;
        this.waterlevel = this.checkWater();
        
        if(oldLevel == WL_UNDER && this.waterlevel == WL_BETWEEN)
        {
            // TODO - determine if client or server
            if(this.local == true)
                Snd.play('sounds/shaders/generic_16_turok_small_water_gasp.ksnd');
        }
        
        if(this.waterlevel == WL_INVALID || this.waterlevel == WL_OVER)
            return false;
        
        if(this.waterlevel == WL_UNDER && oldLevel != WL_UNDER)
        {
            // TODO - determine if client or server
            if(this.local == true)
                Snd.play('sounds/shaders/water_splash_1.ksnd');
        }
        
        if(this.state != STATE_MOVE_SWIM)
        {
            this.accel.y = this.velocity.y;
            return true;
        }
        
        return true;
    }
    
    this.swimMove = function()
    {
        var pitch = this.angles.pitch;
        
        if(this.waterlevel == WL_BETWEEN)
        {
            if(pitch < Angle.degToRad(45))
                pitch = 0;
        }
        
        this.setDirection(
            pitch,
            this.angles.yaw,
            0);
            
        var roll = 0;
        
        // roll the player's view when strafing or turning while underwater
        if(this.waterlevel == WL_UNDER)
        {
            var t = 60 * this.frametime;
            
            if(t != 0)
                roll = this.command.mouse_x * 0.05 * 1 / t;
        
            if(roll > SWIMROLL_MAXANGLE)
                roll = SWIMROLL_MAXANGLE;
                
            if(roll < -SWIMROLL_MAXANGLE)
                roll = -SWIMROLL_MAXANGLE;
        }
        
        if(this.command.getAction('+strafeleft'))
            roll += -(SWIMROLL_MAXANGLE / 2);
        else if(this.command.getAction('+straferight'))
            roll += (SWIMROLL_MAXANGLE / 2);
            
        this.angles.roll = Math.lerp(this.angles.roll,
            roll, SWIMROLL_SPEED);
        
        // single forward press causes thrusting
        if((this.command.getAction('+forward') && !this.command.getActionHeldTime('+forward')) &&
            this.movetime < this.timestamp)
        {
            this.movetime = SWIM_MOVETIME + this.timestamp;
            this.accel.z = SWIM_THRUSTSPEED;
            
            // TODO - determine if client or server
            if(this.local == true)
            {
                if(this.waterlevel == WL_UNDER)
                    Snd.play('sounds/shaders/underwater_swim_2.ksnd');
                else
                    Snd.play('sounds/shaders/water_splash_2.ksnd');
            }
        }
        
        if(this.movetime >= this.timestamp)
        {
            // wind down thrusting speed
            this.deAccelZ(SWIM_THRUSTDEACCELERATE);
            this.velocity.add(Vector.gScale(this.forward, this.accel.z));
        }
        else
            this.accelerate(this.speed[STATE_MOVE_SWIM]);
        
        // handle jumping while in water. handle vertical movement if underwater
        if(this.waterlevel == WL_UNDER)
        {
            if(this.command.getAction('+jump'))
                this.accelY(SWIM_UPSPEED, SWIM_ACCELERATION);
            else
                this.deAccelY(SWIM_UPDEACCELERATION);
                
            this.velocity.y += this.accel.y;
        }
        else
        {
            if(this.command.getAction('+jump') && !this.command.getActionHeldTime('+jump'))
                this.jump();
        }
        
        // swimming sounds
        if(this.accel.unit3() >= 3)
        {
            if(!(Sys.ticks() % 100))
            {
                // TODO - determine if client or server
                if(this.local == true)
                {
                    if(this.waterlevel == WL_UNDER)
                        Snd.play('sounds/shaders/underwater_swim_1.ksnd');
                    else
                        Snd.play('sounds/shaders/water_splash_3.ksnd');
                }
            }
        }
        
        if(this.waterlevel == WL_BETWEEN)
        { 
            if(this.velocity.y < 0 && this.velocity.y > -250.0)
            {
                this.accel.y = 0;
                this.velocity.y = 0;
            }
        }
        
        this.bLerping = false;
        
        // check if we need to lerp to the surface
        if(this.waterlevel == WL_UNDER && this.accel.y > 0)
        {
            var viewy = this.origin.y + this.center_y + 
                this.view_y;
            
            if(viewy > this.waterheight)
            {
                this.bLerping = true;
                
                this.origin.y =
                    this.lerp(this.origin.y, this.waterheight, 0.1);
                
                if(this.origin.y >= (this.waterheight - this.center_y))
                {
                    this.accel.y = 0;
                    this.velocity.y = 0;
                }
            }
        }
        
        // slowly drift towards the bottom, but will remain floating slightly
        // above the floor/terrain
        if(!this.bLerping && this.frametime != 0.0)
        {
            if(this.waterlevel == WL_UNDER &&
                (this.velocity.y < 0.125 && this.velocity.y > -0.125))
            {
                var sink = this.origin.y - this.getSinkHeight();
                
                if(sink * 0.125 >= 2.0)
                    sink = sink * 0.125;
                else
                    sink = 2.0;
                
                this.velocity.y =
                    this.lerp(this.velocity.y, (sink * 4) / this.frametime, -0.0035);
            }
        }
        
        this.super.prototype.beginMovement.bind(this)();
        
        this.applyFriction(MOVE_FRICTION);
        
        if(this.waterlevel == WL_UNDER)
            this.velocity.y = 0;
    }
});

class.properties(ControllerPlayer,
{
    beginMovement : function()
    {
        if(this.frametime >= 1)
            return;
        
        if(this.bNoClip == true)
        {
            this.noClipMove();
            return;
        }
        
        if(this.bFlying == true)
        {
            this.flyMove();
            return;
        }
        
        if(this.plane == null)
            return;
        
        var area = this.plane.area;
        
        if(this.inWater())
        {
            this.state = STATE_MOVE_SWIM;
        }
        else if(this.canClimb() && !this.canClimbLeap())
        {
            this.state = STATE_MOVE_CLIMB;
        }
        else if(this.canJump())
        {
            this.state = STATE_MOVE_AIR;
        }
        else
        {
            this.state = STATE_MOVE_WALK;
        }
        
        switch(this.state)
        {
            case STATE_MOVE_WALK:
                this.walkMove();
                break;
                
            case STATE_MOVE_AIR:
                this.airMove();
                break;
                
            case STATE_MOVE_CLIMB:
                this.climbMove();
                break;
                
            case STATE_MOVE_SWIM:
                this.swimMove();
                break;
        }
        
        if(area != this.plane.area)
        {
            var component;
            
            for(component in area)
                area[component].onExit(this);
            
            for(component in this.plane.area)
                this.plane.area[component].onEnter(this);
        }
    },
    
    hitFloor : function()
    {
    }
});
