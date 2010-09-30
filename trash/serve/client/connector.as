import gui_button;
import gui_input;

class connector extends cobj {
    private static var DEFAULT_HOST:String = "localhost";
    private static var DEFAULT_PORT:Number = 7000;

    private static var STATE_IDLE      :Number = 0;
    private static var STATE_CONNECTING:Number = 1;
    private static var STATE_CONNECTED :Number = 2;

    private var input_host:gui_input = null;
    private var input_port:gui_input = null;
    
    private var button_connect:gui_button     = null;
    private var button_connect_labels  :Array = null;
    private var button_connect_handlers:Array = null;

    public var retry:Boolean      = true;
    public var attempt:Number     = 0;

    private var state:Number = 0;

    private var socket:XMLSocket   = null;
    private var int_connect:Number = 0;
    private var int_send:Number    = 0;
    private var queue_send:Array   = null;

    public var server_host:String = null;
    public var server_port:Number = null;

    public var handle_data      :Function = null;
    public var handle_connect   :Function = null;
    public var handle_disconnect:Function = null;

    function connector() {
	super();

	var self = this;

	this.state = connector.STATE_IDLE;
	this.button_connect_labels   = ["connect", "cancel", "disconnect"];
	this.button_connect_handlers = [function(){self.connect()},
					function(){self.cancel()},
					function(){self.disconnect()}];

	this.init_socket();
	this.queue_send = new Array();

	this.create_content();

	return this;
    }

    public static function create(mc:MovieClip, mcn:String): connector {
	return cobj.create_common(mc, "connector", mcn);
    }

    private function init_socket():Void {
	this.socket = new XMLSocket();

	var self = this;

	this.socket.onClose = function() {
	    self.state = connector.STATE_IDLE;

	    self.attempt = 0;
	    
	    trace("disconnected");

	    if (self.retry) {
		self.state = connector.STATE_CONNECTING;
		self.handle_disconnect();
		self.connect();

	    } else {
		self.attempt = 0;
		self.handle_disconnect();
	    }

	    self.update_content();
	}

	this.socket.onConnect = function(res) {
	    if (res) {
		clearInterval(self.int_connect);
		self.int_connect = 0;

		self.state = connector.STATE_CONNECTED;

		trace("connected");

		self.send("serve/0.1");

		self.handle_connect();

	    } else if (! self.retry) {
		self.state = connector.STATE_IDLE;

		clearInterval(self.int_connect);
		self.int_connect = 0;
		self.attempt     = 0;

	    } else if (! self.int_connect) {
		self.int_connect = setInterval(self.connect, 3000);

	    }

	    self.update_content();
	}

	this.socket.onData = function(src:String) {
	    trace("received \"" + src + "\"");
	    //	    trace("raw " + self.raw_text(src));	    

	    //self.handle_data(src);
	}
    }

    public function connect():Void {
	if (connector.STATE_CONNECTED == this.state) {
	    trace("already connected");
	    return;
	}

	var self = this;

	this.retrieve_input();

	this.attempt++;
	this.state = connector.STATE_CONNECTING;

	trace("connect: connecting to " + this.server_host + ":" + this.server_port + " (attempt " + this.attempt + ")");

	var res = this.socket.connect(this.server_host, this.server_port);

	/*
	if ((! this.connecting) &&
	    (! this.connected)) {
	    trace("connect: already connected or connecting");
	    return;
	}
	*/

	if (! res) {
	    trace("connect: bad address");
	    this.attempt = 0;
	    this.state = connector.STATE_IDLE;

	} else if (! this.int_connect) {
	    this.int_connect = setInterval(function(){self.connect()}, 3000);
	}

	this.update_content();
    }

    public function cancel():Void {
	if (connector.STATE_CONNECTING != this.state) {
	    trace("not connecting");
	    return;
	}

	clearInterval(this.int_connect);
	this.int_connect = 0;
	this.attempt = 0;
	this.state = connector.STATE_IDLE;

	this.update_content();
    }

    public function disconnect():Void {
	this.socket.close();
	//	this.connected = false;
	this.state = connector.STATE_IDLE;
	this.attempt = 0;
	this.handle_disconnect();

	this.update_content();
    }

    function send(data:String) {
	if (connector.STATE_CONNECTED != this.state) { 
	    trace("not connected");
	    return;
	}

	trace("queueing [" + data + "]");

	this.queue_send.push(data);

	if(this.int_send) {
	    //	    trace("send: already waiting to send");
	    return;
	}

	this.int_send = setInterval(this.send_queue, 100, this);
    }

    function send_queue(self:connector) {
	if(self.queue_send.length == 0) {
	    //	    trace("send_queue: empty queue");
	    clearInterval(self.int_send);
	    self.int_send = 0;
	    return;
	}

	for (var i:Number = 0; i < self.queue_send.length; i++) {
	    self.socket.send(self.queue_send[i] + "\n");
	    trace("sending [" + self.queue_send[i] + "]");
	}
	//	var msg:String = self.queue_send.join(String.fromCharCode(0));
	self.queue_send = new Array();
	//	self.socket.send(msg);

	//trace("send_queue: sending:" + msg);
	
	if (! self.int_send) {
	    self.int_send = setInterval(self.send_queue, 1000, self);
	}
    }

    /* content */

    private function retrieve_input():Void {
	this.server_host = this.input_host.text;
	this.server_port = Number(this.input_port.text);

	return;
    }

    private function create_content():Void {
	var self:connector = this;

	this.create_bg();
	this.draw_bg(0xbbbbbb);
	this.draw_border(0x000000);
	super.set_size(70,80);

	//this.input_host = new gui_input(this, "mc_input_host");
	this.input_host = gui_input.create(this, "input_host_mc");
	this.input_host.set_geo(5,5,60,20);
	this.input_host.set_text(connector.DEFAULT_HOST);

	//this.input_port = new gui_input(this, "mc_input_port");
	this.input_port = gui_input.create(this, "input_port_mc");
	this.input_port.set_geo(5,30,60,20);
	this.input_port.set_text(String(connector.DEFAULT_PORT));

	//this.button_connect = new gui_button(this.content, "mc_button_connect");
	this.button_connect = gui_button.create(this, "button_connect_mc");
	this.button_connect.set_geo(5,55,60,20);
	this.button_connect.set_text("connect");

	this.button_connect.handle_press = function(b:gui_button) {
	    self.button_connect_handlers[self.state]();
	    /*
	      if (connector.STATE_IDLE == self.state) {
	      self.connect();
	      } else if (connector.STATE_CONNECTING == self.state) {
	      self.cancel();
	      } else if (connector.STATE_CONNECTED == self.state) {
	      self.disconnect();
	      }
	    */
	}

	return;
    }

    public function update_content():Void {
	this.button_connect.set_text(this.button_connect_labels[this.state]);

	/*
	  if (connector.STATE_IDLE == this.state) {
	  this.button_connect.set_text("connect");	    
	  } else if (connector.STATE_CONNECTING == this.state) {
	  this.button_connect.set_text("cancel");
	  } else if (connector.STATE_CONNECTED == this.state) {
	  this.button_connect.set_text("disconnect");
	  }
	*/

    }

    public function set_size(w:Number,h:Number): Void {
	//super.set_size(w,h); // size is fixed

	return;
    }
    
    public function set_geo(x:Number,y:Number,w:Number,h:Number) {
	this.set_pos(x,y);
	//this.set_size(w,h); // size is fixed

	return;
    }

    /* end content */

}
