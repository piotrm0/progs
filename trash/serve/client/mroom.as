import part;

class mroom extends cobj {
    private var r:room = null;

    public function mroom() {
	super();

	room.create(this, "r");

	return this;
    }

    public static function create(mc:MovieClip, mcn:String):mroom {
	return cobj.create_common(mc, "mroom", mcn);
    }

    public function fill_random():Void {
	for (var i:Number = 0; i < 100; i++) {
	    var temp:part = this.r.add_part();
	    temp.set_pos(Math.random()*100,
			 Math.random()*100);
	}

	return;
    }
}
