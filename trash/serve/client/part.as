class part extends MovieClip {
    private var text_label:TextField = null;

    private var pos:Array = null;
    private var vel:Array = null;

    private var pos_map:Array = null;

    public function part() {
	super();
	
	this.pos = [0.0, 0.0, 0.0];
	this.vel = [0.0, 0.0, 0.0];

	this.pos_map = [0.0, 0.0,  // translate
			1.0, 1.0]; // scale

	trace("part made");

	return this;
    }

    public static function create(mc:MovieClip, mcn:String):part {
	return part.create_common(mc, "part", mcn);
    }

    public static function create_common(mc:MovieClip, mcc:String, mcn:String) {
	mc.attachMovie(mcc, mcn, mc.getNextHighestDepth());

	return mc[mcn];
    }

    public function tick(dt:Number):Void {
	this.pos[0] += this.vel[0] * dt;
	this.pos[1] += this.vel[1] * dt;

	return;
    }

    private function update_pos():Void {
	this._x = this.pos[0] * this.pos_map[2] + this.pos_map[0];
	this._y = this.pos[1] * this.pos_map[3] + this.pos_map[1];

	return;
    }

    public function set_pos(x:Number, y:Number):Void {
	this.pos[0] = x;
	this.pos[1] = y;

	this.update_pos();

	return;
    }

    public function set_vel(xv:Number, yv:Number):Void {
	this.vel[0] = xv;
	this.vel[1] = yv;

	return;
    }
}
