class room extends cobj {
    private var parts:Array = null;
    private var focus:part  = null;

    private var last_part:Number = 0;

    public function room() {
	super();

	this.parts = new Array();

	return this;
    }

    public static function create(mc:MovieClip, mcn:String):room {
	return cobj.create_common(mc, "room", mcn);
    }

    public function add_part():part {
	var temp:part = part.create(this, "part_" + (last_part++));
	this.parts.push(temp);
	return temp;
    }

    public function tick(dt:Number):Void {
	for (var i:Number = 0; i < this.parts.length; i++) {
	    this.parts[i].tick(dt);
	}
    }


}
