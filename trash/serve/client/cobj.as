package {

class cobj extends MovieClip {
    private static var PADDING: Number = 5;

    private var bg:MovieClip     = null;

    private var x:Number;
    private var y:Number;
    private var width :Number;
    private var height:Number;

    function cobj() {
	super();

	//mc.createEmptyMovieClip(mcn, mc.getNextHighestDepth());
	//this.content = mc[mcn];

	return this;
    }

    public static function create(mc:MovieClip, mcn:String): cobj {
	return cobj.create_common(mc, "cobj", mcn);
    }

    public static function create_common(mc:MovieClip, mcc:String, mcn:String) {
	mc.attachMovie(mcc, mcn, mc.getNextHighestDepth());

	return mc[mcn];
    }

    public function create_bg(): Void {
	this.bg = this.createEmptyMovieClip("bg", this.getNextHighestDepth());

	return;
    }

    public function draw_bg(color:Number) {
	this.bg.lineStyle(0,0,0);
	this.bg.beginFill(color, 100);
	this.bg.moveTo(0,0);
	this.bg.lineTo(100,0);
	this.bg.lineTo(100,100);
	this.bg.lineTo(0,100);
	this.bg.endFill();

	return;
    }

    public function draw_border(color:Number):Void {
	this.bg.lineStyle(1,color,100);
	this.bg.moveTo(0,0);
	this.bg.lineTo(100,0);
	this.bg.lineTo(100,100);
	this.bg.lineTo(0,100);
	this.bg.lineTo(0,0);

	return;
    }

    public function set_vis(v:Boolean): Void {
	this._visible    = v;
	//this.bg._visible = v;

	return;
    }

    public function set_pos(x:Number,y:Number): Void {
	this.x = x;
	this.y = y;

	this._x = x;
	this._y = y;

	return;
    }

    public function set_size(w:Number,h:Number): Void {
	this.width  = w;
	this.height = h;

	this.bg._width  = w;
	this.bg._height = h;

	return;
    }

    public function set_geo(x:Number,y:Number,w:Number,h:Number) {
	this.set_pos(x,y);
	this.set_size(w,h);

	return;
    }
}

}
