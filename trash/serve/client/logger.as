class logger extends cobj {
    private var text_main:TextField = null;

    private var max_lines   :Number = 45;
    private var current_line:Number = 0;
    private var current_num :Number = 0;
    private var lines       :Array  = null;

    function logger() {
	super();

	this.create_content();
	this.init_log();

	return this;
    }

    public static function create(mc:MovieClip, mcn:String):logger {
	return cobj.create_common(mc, "logger", mcn);
    }

    private function init_log(): Void {
	this.lines     = new Array();

	for (var i:Number = 0; i < this.max_lines; i++) {
	    this.lines[i] = "";
	}

	return;
    }

    public function log_labeled(label:String, text:String):Void {
	this.lines[this.current_line] = "[" + this.current_num + "] " + label + "\t" + text;
	this.update_text();

	return;
    }

    public function log(text:String):Void {
	this.lines[this.current_line] = "[" + this.current_num + "]\t" + text;
	this.update_text();

	return;
    }

    /* content */

    private function create_content():Void {
	this.create_bg();
	this.draw_bg(0xdddddd);
	this.createTextField("text_main",
			     this.getNextHighestDepth(),
			     cobj.PADDING, cobj.PADDING,
			     300,          300);
	this.text_main.wrap = true;

	return;
    }

    public function set_size(w:Number, h:Number):Void {
	super.set_size(w,h);

	this.text_main._width  = w - cobj.PADDING * 2;
	this.text_main._height = h - cobj.PADDING * 2;

	return;
    }
    
    private function update_text(): Void {
	this.current_num  ++;
	this.current_line ++;
	this.current_line %= this.max_lines;
	this.text_main.text = this.lines.join("\n");

	return;
    }

    /* end content */
}
