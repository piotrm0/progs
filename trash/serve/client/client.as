import connector;
import logger;

import mroom;
import room;
import part;

class client extends cobj {
    public var conn:connector = null;
    public var log :logger    = null;
    public var mr  :mroom     = null;

    function client() {
	super();

	this.create_logger();
	this.create_connector();
	this.create_mroom();

	return this;
    }

    public static function create(mc:MovieClip, mcn:String):cobj {
	return cobj.create_common(mc, "client", mcn);
    }

    private function create_connector():Void {
	var self = this;

	connector.create(this, "conn");
	this.conn.set_geo(100,100,300,300);

	this.conn.handle_connect = function() {
	    this.set_vis(false);
	};
	this.conn.handle_disconnect = function() {
	    this.set_vis(true);
	};

	return;
    }

    private function create_logger():Void {
	logger.create(this, "log");
	this.log.set_geo(400,0,400,600);

	return;
    }

    private function create_mroom():Void {
	mroom.create(this, "mr");

	this.mr.set_geo(0,0,400,600);

	this.mr.fill_random();

	return;
    }

    static function custom_trace(text:String, class_name:String, file_name:String, line:Number):Void {
	_root['client_mc'].log.log_labeled( file_name + ":" + line, text);
    }

    /*
    static function main(mc:MovieClip):Void {
	var app:client = new client(_root, "client_mc");
	_root.client_instance = app;
	trace("main");

	var bg:MovieClip = _root.client_instance.content.createEmptyMovieClip("bg", _root.client_instance.content.getNextHighestDepth());
	//var bg:MovieClip = this.content.bg;

	trace("attaching part");
	mc.attachMovie("part", "part_01", mc.getNextHighestDepth());

	//app.log.create_bg();
    }
    */
}
