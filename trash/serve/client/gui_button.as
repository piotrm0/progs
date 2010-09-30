class gui_button extends gui {
    public var handle_press:Function = null;

    private var text_label:TextField   = null;
    private var text_format:TextFormat = null;

    function gui_button() {
	super();

	this.create_content();

	return this;
    }

    public static function create(mc:MovieClip, mcn:String): gui_button {
	return cobj.create_common(mc, "gui_button", mcn);
    }

    /* content */

    private function create_content():Void {
	var self:gui_button = this;

	this.create_bg();
	this.draw_bg(0xbbbbbb);
	this.draw_border(0x888888);

	this.bg.onPress = function() {
	    self.handle_press(self);
	}

	this.text_format = new TextFormat();
	this.text_format.color = 0x333333;
	this.text_format.bullet    = false;
	this.text_format.underline = false;
	this.text_format.align     = "center";
	this.text_format.size      = 40 - (2 * gui.PADDING - 2);

	this.createTextField("text_label", this.getNextHighestDepth(), 0,gui.PADDING*0.5,200,40 - gui.PADDING);
	//this.text_label = this.content['text_label'];
	this.text_label.multiline = false;
	this.text_label.wordWrap  = false;
	this.text_label.border    = false;
	this.text_label.selectable = false;
	//this.text_label.autoSize = true;

	return;
    }

    private function update_text():Void {
	var ext:Object = this.text_format.getTextExtent(this.text_label.text);

	var height = ext.height;

	this.text_label._y = (this.height - height) / 2 - 1;

	this.text_label.setTextFormat(this.text_format);

	return;
    }

    public function set_size(w:Number,h:Number): Void {
	super.set_size(w,h);

	this.text_label._width  = w;
	this.text_label._height = h - gui.PADDING;
	this.text_format.size = h - 2 * gui.PADDING + 2;

	this.update_text();
       
	return;
    }

    public function set_text(text:String): Void {
	this.text_label.text = text;

	this.update_text();
       
	return;
    }
    
    /* end content */

}
