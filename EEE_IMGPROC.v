module EEE_IMGPROC(
	// global clock & reset
	clk,
	reset_n,
	
	// mm slave
	s_chipselect,
	s_read,
	s_write,
	s_readdata,
	s_writedata,
	s_address,

	// stream sink
	sink_data,
	sink_valid,
	sink_ready,
	sink_sop,
	sink_eop,
	
	// streaming source
	source_data,
	source_valid,
	source_ready,
	source_sop,
	source_eop,
	
	// conduit
	mode
	
);


// global clock & reset
input	clk;
input	reset_n;

// mm slave
input							s_chipselect;
input							s_read;
input							s_write;
output	reg	[31:0]	s_readdata;
input	[31:0]				s_writedata;
input	[2:0]					s_address;


// streaming sink
input	[23:0]            	sink_data;
input								sink_valid;
output							sink_ready;
input								sink_sop;
input								sink_eop;

// streaming source
output	[23:0]			  	   source_data;
output								source_valid;
input									source_ready;
output								source_sop;
output								source_eop;

// conduit export
input                         mode;

////////////////////////////////////////////////////////////////////////
//
parameter IMAGE_W = 11'd640;
parameter IMAGE_H = 11'd480;
parameter MESSAGE_BUF_MAX = 256;
parameter MSG_INTERVAL = 6;
parameter BB_COL_DEFAULT_RED = 24'hff0000;
parameter BB_COL_DEFAULT = 24'h00ff00;
parameter BB_COL_DEFAULT_BLUE = 24'h0000ff;
parameter BB_COL_DEFAULT_ORANGE = 24'hde9a07;
parameter BB_COL_DEFAULT_PINK   = 24'hffbafd;  
parameter BB_COL_DEFAULT_GREEN   = 24'h00ff00; 
// pink 8'hff, 8'hba, 8'hfd     orange  {8'hde, 8'h9a, 8'h7}

wire [7:0]   red, green, blue, grey;
wire [7:0]   red_out, green_out, blue_out;

wire         sop, eop, in_valid, out_ready;
////////////////////////////////////////////////////////////////////////
// HSV 
wire [7:0] hue_init , hue_mid, min , max ,  sat, value, hue;
assign max = (red >blue) ? ((red>green) ? red : green) : (blue > green) ? blue : green; // max(R,G,B)
assign value = max;    // val = MAX
assign min = (red < blue) ? ((red<green) ? red : green) : (blue < green) ? blue : green;
assign sat = (value != 0) ? (value - min)* 255 / value : 0;


// base hue 
assign hue_init = (red == green && red == blue) ? 0 :((value != red)? (value != green) ? (((240*((value - min))+ (60* (red - green)))/(value-min))>>1):
                ((120*(value-min)+60*(blue - red))/(value - min)): 
                (blue < green) ? ((60*(green - blue)/(value - min))): (((360*(value-min) +(60*(green - blue)))/(value - min))));
assign hue_mid = (hue_init<0) ? hue_init + 360 : hue_init;
	 
assign hue = hue_mid>>1;

					 
/// Detect  areas
wire red_detect , orange_detect, pink_detect, blue_detect, any_detect , green_detect;
reg prev_r0, prev_r1 , prev_r2 ,prev_r3 , prev_r4;  // last 5 red detects
reg prev_o0, prev_o1 , prev_o2 ,prev_o3, prev_o4;  // last 5 orange detects
reg prev_b0, prev_b1 , prev_b2 ,prev_b3, prev_b4;  // last 5 blue detects
reg prev_p0, prev_p1 , prev_p2 ,prev_p3, prev_p4;  // last 5 pink detects
reg prev_g0, prev_g1 , prev_g2 ,prev_g3, prev_g4;  // last 5 green detects

initial begin
	prev_r0 <=0;
	prev_r1 <=0;
	prev_r2 <=0;
	prev_r3 <=0;
	prev_r4 <=0;
	
	prev_o0 <=0;
	prev_o1 <=0;
	prev_o2 <=0;
	prev_o3 <=0;
	prev_o4 <=0;
	
	prev_b0 <=0;
	prev_b1 <=0;
	prev_b2 <=0;
	prev_b3 <=0;
	prev_b4 <=0;
	
	prev_p0 <=0;
	prev_p1 <=0;
	prev_p2 <=0;
	prev_p3 <=0;
	prev_p4 <=0;
	
	prev_g0 <=0;
	prev_g1 <=0;
	prev_g2 <=0;
	prev_g3 <=0;
	prev_g4 <=0;
end


always@(posedge clk) begin
	prev_r0 <= red_detect;
	prev_r1 <=prev_r0;
	prev_r2 <=prev_r1;
	prev_r3 <=prev_r2;
	prev_r4 <= prev_r3;
	
	
	prev_o0 <=orange_detect;
	prev_o1 <=prev_o0;
	prev_o2 <=prev_o1;
	prev_o3 <=prev_o2;
	prev_o4 <= prev_o3;
	
	prev_b0 <=blue_detect;
	prev_b1 <=prev_b0;
	prev_b2 <=prev_b1;
	prev_b3 <=prev_b2;
	prev_b4 <= prev_b3;
	
	prev_p0 <=pink_detect;
	prev_p1 <=prev_p0;
	prev_p2 <=prev_p1;
	prev_p3 <=prev_p2;
	prev_p4 <= prev_p3;
	
	prev_g0 <=green_detect;
	prev_g1 <=prev_g0;
	prev_g2 <=prev_g1;
	prev_g3 <=prev_g2;
	prev_g4 <= prev_g3;
	
end
	
	
// COLOUR DETECTION USING MIXTURE OF HUES AND RGB CONDITIONS

assign   pink_detect     = (((hue >= 0 && hue <= 15)||(hue >= 165 && hue <= 180)) && value > 120 && sat > 110);
assign   orange_detect  = (hue >= 15 && hue <= 30 && value > 120 && sat > 110);
assign   blue_detect    = ((blue>green-20) && (blue>red) && ~pink_detect && ~orange_detect );//(hue >= 55 && hue <= 85 && saturation >= 51 && sat <= 89 && value >= 76 && value <= 240);
assign   red_detect    = ((red>green+30) && (red>blue+30) && ~pink_detect && ~orange_detect);//(((hue >= 0 && hue <= 15)||(hue >= 165 && hue <= 180)) && value > 120 && sat > 110);
assign 	green_detect = ((green>blue+10) && (green>red+30));
assign   any_detect = ((pink_detect || orange_detect || blue_detect || red_detect || green_detect ));



// Find boundary of cursor box

// Highlight detected areas
wire [23:0] red_high;
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign red_high  =  (red_detect && prev_r0 && prev_r1 && prev_r2 && prev_r3  && prev_r4) ? {8'hff,8'h0,8'h0} 
	: ((blue_detect && prev_b0 && prev_b1 && prev_b2 && prev_b3)? {8'h0,8'h0,8'hff} 
	: ((orange_detect && prev_o0 && prev_o1 && prev_o2 && prev_o3 && prev_o4)? {8'hde, 8'h9a, 8'h7}
	: ((pink_detect && prev_p0 && prev_p1 && prev_p2 && prev_p3 && prev_p4)? {8'hff, 8'hba, 8'hfd}
	: ((green_detect && prev_g0 && prev_g1 && prev_g2 && prev_g3 && prev_g4)? {8'h00, 8'hff, 8'h00}
	: {grey,grey,grey}))));

	
// Show bounding box RED
wire [23:0] new_image; //, new_image_r, new_image_b, new_image,p, new_immage_o;
wire bb_active_r , bb_active_b, bb_active_p , bb_active_o , bb_active_g;
assign bb_active_r = (x == left_r) | (x == right_r) | (y == top_r) | (y == bottom_r);
assign bb_active_p = (x == left_p) | (x == right_p) | (y == top_p) | (y == bottom_p);
assign bb_active_b = (x == left_b) | (x == right_b) | (y == top_b) | (y == bottom_b);
assign bb_active_o = (x == left_o) | (x == right_o) | (y == top_o) | (y == bottom_o);
assign bb_active_g = (x == left_g) | (x == right_g) | (y == top_g) | (y == bottom_g);
//assign new_image_r = bb_active_r ? bb_col_r : red_high;

assign new_image  =  (bb_active_r) ? bb_col_r  
	: ((bb_active_p)? bb_col_p
	: ((bb_active_b)? bb_col_b
	: ((bb_active_o)? bb_col_o
	: red_high)));

// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet discriptor
// Don't modify data in non-video packets
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video) ? new_image : {red,green,blue};

//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x, y;
reg packet_video;
always@(posedge clk) begin
	if (sop) begin
		x <= 11'h0;
		y <= 11'h0;
		packet_video <= (blue[3:0] == 3'h0);
	end
	else if (in_valid) begin
		if (x == IMAGE_W-1) begin
			x <= 11'h0;
			y <= y + 11'h1;
		end
		else begin
			x <= x + 11'h1;
		end
	end
end

//Find first and last  pixels
reg [10:0] x_min_r, y_min_r, x_max_r, y_max_r;
reg [10:0] x_min_o, y_min_o, x_max_o, y_max_o;
reg [10:0] x_min_b, y_min_b, x_max_b, y_max_b;
reg [10:0] x_min_p, y_min_p, x_max_p, y_max_p;
reg [10:0] x_min_g, y_min_g, x_max_g, y_max_g;
reg [10:0] x_min, y_min, x_max, y_max; // THIS IS JUST THERE AS IT IS USED IN THE MESSAGE FUNCTIONS
wire  bound_orange , bound_pink, bound_red, bound_blue, bound_green;
assign bound_orange = orange_detect && prev_o0 && prev_o1 && prev_o2 && prev_o3  && prev_o4;
assign bound_red = red_detect && prev_r0 && prev_r1 && prev_r2 && prev_r3  && prev_r4;
assign bound_blue = blue_detect && prev_b0 && prev_b1 && prev_b2 && prev_b3;
assign bound_pink = pink_detect && prev_p0 && prev_p1 && prev_p2 && prev_p3  && prev_p4;
assign bound_green = green_detect && prev_g0 && prev_g1 && prev_g2 && prev_g3  && prev_g4;

// RED BOX
always@(posedge clk) begin
	if ((bound_red && in_valid)) begin	//Update bounds when the pixel is red
		if (x < x_min_r) x_min_r <= x;
		if (x > x_max_r) x_max_r <= x;
		if (y < y_min_r) y_min_r <= y;
		y_max_r <= y;
	end
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_r <= IMAGE_W-11'h1;
		x_max_r <= 0;
		y_min_r <= IMAGE_H-11'h1;
		y_max_r <= 0;
	end
end

// BLUE BOX
always@(posedge clk) begin
	if ((bound_blue && in_valid)) begin	//Update bounds when the pixel is blue
		if (x < x_min_b) x_min_b <= x;
		if (x > x_max_b) x_max_b <= x;
		if (y < y_min_b) y_min_b <= y;
		y_max_b <= y;
	end
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_b <= IMAGE_W-11'h1;
		x_max_b <= 0;
		y_min_b <= IMAGE_H-11'h1;
		y_max_b <= 0;
	end
end

// ORANGE BOX
always@(posedge clk) begin
	if ((bound_orange && in_valid)) begin	//Update bounds when the pixel is orange
		if (x < x_min_o) x_min_o <= x;
		if (x > x_max_o) x_max_o <= x;
		if (y < y_min_o) y_min_o <= y;
		y_max_o <= y;
	end
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_o <= IMAGE_W-11'h1;
		x_max_o <= 0;
		y_min_o <= IMAGE_H-11'h1;
		y_max_o <= 0;
	end
end

// PINK BOX
always@(posedge clk) begin
	if ((bound_pink && in_valid)) begin	//Update bounds when the pixel is pink
		if (x < x_min_p) x_min_p <= x;
		if (x > x_max_p) x_max_p <= x;
		if (y < y_min_p) y_min_p <= y;
		y_max_p <= y;
	end
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_p <= IMAGE_W-11'h1;
		x_max_p<= 0;
		y_min_p <= IMAGE_H-11'h1;
		y_max_p <= 0;
	end
end

// GREEN BOX
always@(posedge clk) begin
	if ((bound_green && in_valid)) begin	//Update bounds when the pixel is pink
		if (x < x_min_g) x_min_g <= x;
		if (x > x_max_g) x_max_g <= x;
		if (y < y_min_g) y_min_g <= y;
		y_max_g <= y;
	end
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_g <= IMAGE_W-11'h1;
		x_max_g<= 0;
		y_min_g <= IMAGE_H-11'h1;
		y_max_g <= 0;
	end
end




//Process bounding box at the end of the frame.
reg [1:0] msg_state;
reg [10:0] left_r, right_r, top_r, bottom_r;
reg [10:0] left_b, right_b, top_b, bottom_b;
reg [10:0] left_o, right_o, top_o, bottom_o;
reg [10:0] left_p, right_p, top_p, bottom_p;
reg [10:0] left_g, right_g, top_g, bottom_g;
reg [7:0] frame_count;
always@(posedge clk) begin
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets
		
		//Latch edges for display overlay on next frame  (RED BOX)
		left_r <= x_min_r;
		right_r <= x_max_r;
		top_r <= y_min_r;
		bottom_r <= y_max_r;
		
		//Latch edges for display overlay on next frame  (BLUE BOX)
		left_b <= x_min_b;
		right_b <= x_max_b;
		top_b <= y_min_b;
		bottom_b <= y_max_b;
		
		//Latch edges for display overlay on next frame  (PINK BOX)
		left_p <= x_min_p;
		right_p <= x_max_p;
		top_p <= y_min_p;
		bottom_p <= y_max_p;
		
		//Latch edges for display overlay on next frame  (IORANGE BOX)
		left_o <= x_min_o;
		right_o <= x_max_o;
		top_o <= y_min_o;
		bottom_o <= y_max_o;
		
		//Latch edges for display overlay on next frame  (GREEN BOX)
		left_g <= x_min_g;
		right_g <= x_max_g;
		top_g <= y_min_g;
		bottom_g <= y_max_g;
		
		//Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
		frame_count <= frame_count - 1;
		
		if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
			msg_state <= 2'b01;
			frame_count <= MSG_INTERVAL-1;
		end
	end
	
	//Cycle through message writer states once started
	if (msg_state != 2'b00) msg_state <= msg_state + 2'b01;

end
	
//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;

`define RED_BOX_MSG_ID "RBB"

always@(*) begin	//Write words to FIFO as state machine advances
	case(msg_state)
		2'b00: begin
			msg_buf_in = 32'b0;
			msg_buf_wr = 1'b0;
		end
		2'b01: begin
			msg_buf_in = `RED_BOX_MSG_ID;	//Message ID
			msg_buf_wr = 1'b1;
		end
		2'b10: begin
			msg_buf_in = {5'b0, x_min, 5'b0, y_min};	//Top left coordinate
			msg_buf_wr = 1'b1;
		end
		2'b11: begin
			msg_buf_in = {5'b0, x_max, 5'b0, y_max}; //Bottom right coordinate
			msg_buf_wr = 1'b1;
		end
	endcase
end


//Output message FIFO
MSG_FIFO	MSG_FIFO_inst (
	.clock (clk),
	.data (msg_buf_in),
	.rdreq (msg_buf_rd),
	.sclr (~reset_n | msg_buf_flush),
	.wrreq (msg_buf_wr),
	.q (msg_buf_out),
	.usedw (msg_buf_size),
	.empty (msg_buf_empty)
	);


//Streaming registers to buffer video signal
STREAM_REG #(.DATA_WIDTH(26)) in_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(sink_ready),
	.valid_out(in_valid),
	.data_out({red,green,blue,sop,eop}),
	.ready_in(out_ready),
	.valid_in(sink_valid),
	.data_in({sink_data,sink_sop,sink_eop})
);

STREAM_REG #(.DATA_WIDTH(26)) out_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(out_ready),
	.valid_out(source_valid),
	.data_out({source_data,source_sop,source_eop}),
	.ready_in(source_ready),
	.valid_in(in_valid),
	.data_in({red_out, green_out, blue_out, sop, eop})
);


/////////////////////////////////
/// Memory-mapped port		 /////
/////////////////////////////////

// Addresses
`define REG_STATUS    			0
`define READ_MSG    				1
`define READ_ID    				2
`define REG_BBCOL					3

//Status register bits
// 31:16 - unimplemented
// 15:8 - number of words in message buffer (read only)
// 7:5 - unused
// 4 - flush message buffer (write only - read as 0)
// 3:0 - unused


// Process write

reg  [7:0]   reg_status;
reg	[23:0]	bb_col_r, bb_col_b, bb_col_o, bb_col_p , bb_col , bb_col_g;

always @ (posedge clk)
begin
	if (~reset_n)
	begin
		reg_status <= 8'b0;
		bb_col <= BB_COL_DEFAULT;
		bb_col_r <= BB_COL_DEFAULT_RED;
		bb_col_o <= BB_COL_DEFAULT_ORANGE;
		bb_col_b <= BB_COL_DEFAULT_BLUE;
		bb_col_p <= BB_COL_DEFAULT_PINK;
		bb_col_g <= BB_COL_DEFAULT_GREEN;
	end
	else begin
		if(s_chipselect & s_write) begin
		   if      (s_address == `REG_STATUS)	reg_status <= s_writedata[7:0];
		   if      (s_address == `REG_BBCOL)	bb_col <= s_writedata[23:0];
		end
	end
end


//Flush the message buffer if 1 is written to status register bit 4
assign msg_buf_flush = (s_chipselect & s_write & (s_address == `REG_STATUS) & s_writedata[4]);


// Process reads
reg read_d; //Store the read signal for correct updating of the message buffer

// Copy the requested word to the output port when there is a read.
always @ (posedge clk)
begin
   if (~reset_n) begin
	   s_readdata <= {32'b0};
		read_d <= 1'b0;
	end
	
	else if (s_chipselect & s_read) begin
		if   (s_address == `REG_STATUS) s_readdata <= {16'b0,msg_buf_size,reg_status};
		if   (s_address == `READ_MSG) s_readdata <= {msg_buf_out};
		if   (s_address == `READ_ID) s_readdata <= 32'h1234EEE2;
		if   (s_address == `REG_BBCOL) s_readdata <= {8'h0, bb_col};
	end
	
	read_d <= s_read;
end

//Fetch next word from message buffer after read from READ_MSG
assign msg_buf_rd = s_chipselect & s_read & ~read_d & ~msg_buf_empty & (s_address == `READ_MSG);
						


endmodule
