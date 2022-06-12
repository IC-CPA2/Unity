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
	mode,
	outbuffer,
	received_data_spi,
	received_data_byte_received
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
input		[15:0]					received_data_spi;
output	[15:0]						outbuffer;
input									received_data_byte_received;
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


wire [7:0]   red, green, blue, grey;
wire [7:0]   red_out, green_out, blue_out;

wire         sop, eop, in_valid, out_ready;
////////////////////////////////////////////////////////////////////////
///Declaring variables for HSV conversion
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
reg [7:0] prev_red, prev_green, prev_blue;
reg [7:0] prev_red1, prev_green1, prev_blue1;
reg [7:0] prev_red2, prev_green2, prev_blue2;
reg [7:0] prev_red3, prev_green3, prev_blue3;
reg [7:0] prev_red4, prev_green4, prev_blue4;
reg [7:0] prev_red5, prev_green5, prev_blue5;
reg [7:0] prev_red6, prev_green6, prev_blue6;
reg [7:0] prev_red7, prev_green7, prev_blue7;
reg [7:0] prev_red8, prev_green8, prev_blue8;
reg [7:0] prev_red9, prev_green9, prev_blue9;
reg [7:0] prev_red10, prev_green10, prev_blue10;
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
	
	prev_red <= red;
	prev_green <= green;
	prev_blue <= blue;
	
	prev_red1 <= prev_red;
	prev_green1 <= prev_green;
	prev_blue1 <= prev_blue;
	
	prev_red2 <= prev_red1;
	prev_green2 <= prev_green1;
	prev_blue2 <= prev_blue1;
	
	prev_red3 <= prev_red2;
	prev_green3 <= prev_green2;
	prev_blue3 <= prev_blue2;

	prev_red4 <= prev_red3;
	prev_green4 <= prev_green3;
	prev_blue4 <= prev_blue3;
	
	prev_red5 <= prev_red4;
	prev_green5 <= prev_green4;
	prev_blue5 <= prev_blue4;
	
	prev_red6 <= prev_red5;
	prev_green6 <= prev_green5;
	prev_blue6 <= prev_blue5;
	
	prev_red7 <= prev_red6;
	prev_green7 <= prev_green6;
	prev_blue7 <= prev_blue6;
	
	prev_red8 <= prev_red7;
	prev_green8 <= prev_green7;
	prev_blue8 <= prev_blue7;
	
	prev_red9 <= prev_red8;
	prev_green9 <= prev_green8;
	prev_blue9 <= prev_blue8;
	
	prev_red10 <= prev_red9;
	prev_green10 <= prev_green9;
	prev_blue10 <= prev_blue9;
	
end
	
	
// COLOUR DETECTION USING MIXTURE OF HUES AND RGB CONDITIONS


wire rgb;
assign rgb = (red>green) && (green>blue);
assign pink_detect = ((hue>=1 && hue <= 10) && (sat>=161 && sat <= 191) && (value>=178 && value <= 255))
|| ((hue>=4 && hue <= 15) && (sat>=130 && sat <= 194) && (value>=249 && value <= 255));//(hue>=2 && hue <=16) && (sat>=117 && sat<=210) && (value>=200);//(((hue >= 0 && hue <= 15)||(hue >= 165 && hue <= 180)) && value > 120 && sat > 110);

assign orange_detect  = ((hue>=20 && hue <= 30) && (sat>=107 && sat <= 228) && (value>=248 && value <= 255))
||((hue >= 15 && hue <= 25 && value > 160 && sat > 175));//(hue >= 15 && hue <= 30 && value > 120 && sat > 110);

assign blue_detect    = ((hue>=61 && hue <= 92) && (sat>=60 && sat <= 163) && (value>=46 && value <= 123))
||((hue>=60 && hue <= 90) && (sat>=61 && sat <= 121) && (value>=119 && value <= 236));//((blue>green-20) && (blue>red) && ~pink_detect && ~orange_detect ) || (hue >= 55 && hue <= 85 && sat >= 51 && sat <= 89 && value >= 76 && value <= 240);

assign   red_detect    = ((hue>=5 && hue <= 13) && (sat>=187 && sat <= 254) && (value>=83 && value <= 180))
|| ((hue>=6 && hue <= 15) && (sat>=178 && sat <= 234) && (value>=200 && value <= 255))
|| ((hue>=6 && hue <=18) && (sat>=190 && sat<= 250) && (value>=130 && value<=240)); //(rgb && (red>green+30) && (red>blue+30) && ~pink_detect && ~orange_detect);//(((hue >= 0 && hue <= 15)||(hue >= 165 && hue <= 180)) && value > 120 && sat > 110);

assign 	green_detect = ((green>blue+10) && (green>red+30) && (45 <= hue & hue <= 65));

assign   any_detect = ((pink_detect || orange_detect || blue_detect || red_detect || green_detect ));



// Find boundary of cursor box
wire in_range;
assign in_range = (y>159);



// Highlight detected areas
wire [23:0] red_high;
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign red_high  =  (red_detect && prev_r0 && prev_r1 && prev_r2 && in_range) ? {8'hff,8'h0,8'h0} 
	: ((blue_detect && prev_b0 && prev_b1 && prev_b2 && prev_b3 && in_range)? {8'h0,8'h0,8'hff} 
	: ((orange_detect && prev_o0 && prev_o1 && prev_o2 && prev_o3 && prev_o4 && in_range)? {8'hde, 8'h9a, 8'h7}
	: ((pink_detect && prev_p0 && prev_p1 && prev_p2 && prev_p3 && prev_p4 && in_range)? {8'hff, 8'hba, 8'hfd}
	: ((green_detect && prev_g0 && prev_g1 && prev_g2 && prev_g3 && prev_g4 && in_range)? {8'h00, 8'hff, 8'h00}
	: {grey,grey,grey}))));

	
// Show bounding box RED
wire [23:0] new_image; //, new_image_r, new_image_b, new_image,p, new_immage_o;
wire bb_active_r , bb_active_b, bb_active_p , bb_active_o , bb_active_g, struct_active;
assign bb_active_r = (x == left_r) | (x == right_r) | (y == top_r) | (y == bottom_r);
assign bb_active_p = (x == left_p) | (x == right_p) | (y == top_p) | (y == bottom_p);
assign bb_active_b = (x == left_b) | (x == right_b) | (y == top_b) | (y == bottom_b);
assign bb_active_o = (x == left_o) | (x == right_o) | (y == top_o) | (y == bottom_o);
assign bb_active_g = (x == left_g) | (x == right_g) | (y == top_g) | (y == bottom_g);
//assign struct_active = (x == struct1) | (x == struct2) | (x == struct3) | (x == struct4);

//assign new_image_r = bb_active_r ? bb_col_r : red_high;

assign new_image  =  (bb_active_r) ? bb_col_r  
	: ((bb_active_p)? bb_col_p
	: ((bb_active_b)? bb_col_b
	: ((bb_active_o)? bb_col_o
	: ((struct_bound)? BB_COL_DEFAULT_GREEN
	: red_high))));

// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet discriptor
// Don't modify data in non-video packets
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video) ? new_image : {red,green,blue};

//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x, y;
reg packet_video;
always@(posedge clk) begin
	if (sop && in_valid) begin
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

reg [10:0] struct1, struct2, struct3, struct4;
reg [23:0] prevpix;
wire prev10blckdetect;
wire blckdetect;
wire prev10whtedetect;
wire whtedetect;
wire struct_detect;
wire struct_bound;
assign prev10blckdetect = (prev_red10 <= 78 && prev_green10 <= 78 && prev_blue10 <= 78) && (prev_green10 - prev_red10 < 20) && (prev_green10 - prev_blue10 < 20);
assign prev10whtedetect = (prev_red10 >= 60 && prev_green10 >= 80 && prev_blue10 >= 60) && (prev_green10 - prev_red10 < 20) && (prev_green10 - prev_blue10 < 20);
assign blckdetect = (red <= 78 && green <= 78 && blue <= 78) && (green - red < 20) && (green - blue < 20);
assign whtedetect = (red >= 60 && green >= 80 && blue >= 60) && (green - red < 20) && (green - blue < 20);
assign struct_detect = (prev10blckdetect && !prev10whtedetect && !blckdetect && whtedetect) || (!prev10blckdetect && prev10whtedetect && blckdetect && !whtedetect);
assign struct_bound = (x==struct1 || x==struct2 || x==struct3 || x==struct4);

always@(posedge clk) begin
	prevpix <= {red,green,blue};
	if (sop  && in_valid) begin
		struct1 <= 0;
		struct2 <= 0;
		struct3 <= 0;
		struct4 <= 0;
		// prevpix <= {red,green,blue};
	end
	if(in_valid && (struct1==0 || struct2==0 || struct3==0 || struct4==0)) begin
		if(struct_detect) begin
			if(struct1==0) begin
				struct1 <= x;
			end
			else if(struct2==0 && x-struct1>50 && struct1!=0) begin
				struct2 <= x;
			end
			else if(struct3==0 && x-struct2>50 && struct2!=0) begin
				struct3 <= x;
			end
			else if(struct4==0 && x-struct3>50 && struct3!=0) begin
				struct4 <= x;
			end
		end
	end
end


//Find first and last  pixels
reg [10:0] x_min_r, y_min_r, x_max_r, y_max_r;
reg [10:0] x_min_rt,x_max_rt;

//test for red triple bound
reg [10:0] xb1max, xb1min, xb2max, xb2min, xb3max, xb3min;

//other bounds
reg [10:0] xb1max_r, xb1min_r, xb2max_r, xb2min_r, xb3max_r, xb3min_r;
reg [10:0] xb1max_o, xb1min_o, xb2max_o, xb2min_o, xb3max_o, xb3min_o;
reg [10:0] xb1max_b, xb1min_b, xb2max_b, xb2min_b, xb3max_b, xb3min_b;
reg [10:0] xb1max_p, xb1min_p, xb2max_p, xb2min_p, xb3max_p, xb3min_p;
reg [10:0] xb1max_g, xb1min_g, xb2max_g, xb2min_g, xb3max_g, xb3min_g; 



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

always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_r <= IMAGE_W-11'h1;
		x_max_r <= 0;
		y_min_r <= IMAGE_H-11'h1;
		y_max_r <= 0;
		xb1max_r <= 105;
		xb1min_r <= 104;
		xb2max_r <= 315;
		xb2min_r <= 314;
		xb3max_r <= 530;
		xb3min_r <= 529;
	end
	else begin
		if ((bound_red && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_r || xb1min_r == 104) xb1min_r <= x;
			if (x > xb1max_r || xb1max_r == 105) xb1max_r <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_red && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_r || xb2min_r == 314) xb2min_r <= x;
			if (x > xb2max_r || xb2max_r == 315) xb2max_r <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_red && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_r || xb3min_r == 529) xb3min_r <= x;
			if (x > xb3max_r || xb3max_r == 530) xb3max_r <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		if (((xb3max_r - xb3min_r) > (xb2max_r - xb2min_r)) && ((xb3max_r - xb3min_r) > (xb1max_r - xb1min_r))) begin //b3 biggest
			x_max_r <= xb3max_r;
			if (xb3min_r - xb2max_r < 10) begin //condition when ball overlap 2nd boundary
				x_min_r <= xb2min_r;
			end
			else begin
				x_min_r <= xb3min_r;
			end
		end
		else if ((xb2max_r - xb2min_r > xb3max_r - xb3min_r) && (xb2max_r - xb2min_r > xb1max_r - xb1min_r))begin //b2 biggest
			if (xb2min_r - xb1max_r < 10) begin // condition when ball overalap first boundary
				x_min_r <= xb1min_r;
			end
			else begin
				x_min_r <= xb2min_r;
			end
			
			if (xb3min_r - xb2max_r < 10) begin //condition when ball overlap second boundary
				x_max_r <= xb3max_r; 
			end
			else begin
				x_max_r <= xb2max_r;
			end
		end
		else if ((xb1max_r - xb1min_r > xb3max_r - xb3min_r) && (xb1max_r- xb1min_r > xb2max_r - xb2min_r)) begin //b1 biggest
			x_min_r <= xb1min_r;
			if (xb2min_r - xb1max_r < 10) begin //condition when ball overlap first boundary
				x_max_r <= xb2max_r;
			end
			else begin
				x_max_r <= xb1max_r;
			end
		end
		else begin
			x_max_r <= 0;
			x_min_r <= 0;
		end
	end
end
// BLUE BOX
always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_b <= IMAGE_W-11'h1;
		x_max_b <= 0;
		y_min_b <= IMAGE_H-11'h1;
		y_max_b <= 0;
		xb1max_b <= 105;
		xb1min_b <= 104;
		xb2max_b <= 315;
		xb2min_b <= 314;
		xb3max_b <= 530;
		xb3min_b <= 529;
	end
	else begin
		if ((bound_blue && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_b || xb1min_b == 104) xb1min_b <= x;
			if (x > xb1max_b || xb1max_b == 105) xb1max_b <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_blue && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_b || xb2min_b == 314) xb2min_b <= x;
			if (x > xb2max_b || xb2max_b == 315) xb2max_b <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_blue && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_b || xb3min_b == 529) xb3min_b <= x;
			if (x > xb3max_b || xb3max_b == 530) xb3max_b <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		if (((xb3max_b - xb3min_b) > (xb2max_b - xb2min_b)) && ((xb3max_b - xb3min_b) > (xb1max_b - xb1min_b))) begin //b3 biggest
			x_max_b <= xb3max_b;
			if (xb3min_b - xb2max_b < 10) begin //condition when ball overlap 2nd boundary
				x_min_b <= xb2min_b;
			end
			else begin
				x_min_b <= xb3min_b;
			end
		end
		else if ((xb2max_b - xb2min_b > xb3max_b - xb3min_b) && (xb2max_b - xb2min_b > xb1max_b - xb1min_b))begin //b2 biggest
			if (xb2min_b - xb1max_b < 10) begin // condition when ball overalap first boundary
				x_min_b <= xb1min_b;
			end
			else begin
				x_min_b <= xb2min_b;
			end
			
			if (xb3min_b - xb2max_b < 10) begin //condition when ball overlap second boundary
				x_max_b <= xb3max_b; 
			end
			else begin
				x_max_b <= xb2max_b;
			end
		end
		else if ((xb1max_b - xb1min_b > xb3max_b - xb3min_b) && (xb1max_b - xb1min_b > xb2max_b - xb2min_b)) begin //b1 biggest
			x_min_b <= xb1min_b;
			if (xb2min_b - xb1max_b < 10) begin //condition when ball overlap first boundary
				x_max_b <= xb2max_b;
			end
			else begin
				x_max_b <= xb1max_b;
			end
		end
		else begin
			x_max_b <= 0;
			x_min_b <= 0;
		end
	end
end

// ORANGE BOX
always@(posedge clk) begin
if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_o <= IMAGE_W-11'h1;
		x_max_o <= 0;
		y_min_o <= IMAGE_H-11'h1;
		y_max_o <= 0;
		xb1max_o <= 105;
		xb1min_o <= 104;
		xb2max_o <= 315;
		xb2min_o <= 314;
		xb3max_o <= 530;
		xb3min_o <= 529;
	end
	else begin
		if ((bound_orange && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_o || xb1min_o == 104) xb1min_o <= x;
			if (x > xb1max_o || xb1max_o == 105) xb1max_o <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_orange && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_o || xb2min_o == 314) xb2min_o <= x;
			if (x > xb2max_o || xb2max_o == 315) xb2max_o <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_orange && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_o || xb3min_o == 529) xb3min_o <= x;
			if (x > xb3max_o || xb3max_o == 530) xb3max_o <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		if (((xb3max_o - xb3min_o) > (xb2max_o - xb2min_o)) && ((xb3max_o - xb3min_o) > (xb1max_o - xb1min_o))) begin //b3 biggest
			x_max_o <= xb3max_o;
			if (xb3min_o - xb2max_o < 10) begin //condition when ball overlap 2nd boundary
				x_min_o <= xb2min_o;
			end
			else begin
				x_min_o <= xb3min_o;
			end
		end
		else if ((xb2max_o - xb2min_o > xb3max_o - xb3min_o) && (xb2max_o - xb2min_o > xb1max_o - xb1min_o) )begin //b2 biggest
			if (xb2min_o - xb1max_o < 10) begin // condition when ball overalap first boundary
				x_min_o <= xb1min_o;
			end
			else begin
				x_min_o <= xb2min_o;
			end
			
			if (xb3min_o - xb2max_o < 10) begin //condition when ball overlap second boundary
				x_max_o <= xb3max_o; 
			end
			else begin
				x_max_o <= xb2max_o;
			end
		end
		else if ((xb1max_o - xb1min_o > xb3max_o - xb3min_o) && (xb1max_o - xb1min_o > xb2max_o - xb2min_o)) begin //b1 biggest
			x_min_o <= xb1min_o;
			if (xb2min_o - xb1max_o < 10) begin //condition when ball overlap first boundary
				x_max_o <= xb2max_o;
			end
			else begin
				x_max_o <= xb1max_o;
			end
		end
		else begin
			x_max_o <= 0;
			x_min_o <= 0;
		end
	end
end

// PINK BOX
always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_p <= IMAGE_W-11'h1;
		x_max_p <= 0;
		y_min_p <= IMAGE_H-11'h1;
		y_max_p <= 0;
		xb1max_p <= 105;
		xb1min_p <= 104;
		xb2max_p <= 315;
		xb2min_p <= 314;
		xb3max_p <= 530;
		xb3min_p <= 529;
	end
	else begin
		if ((bound_pink && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_p || xb1min_p == 104) xb1min_p <= x;
			if (x > xb1max_p || xb1max_p == 105) xb1max_p <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_pink && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_p || xb2min_p == 314) xb2min_p <= x;
			if (x > xb2max_p || xb2max_p == 315) xb2max_p <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_pink && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_p || xb3min_p == 529) xb3min_p <= x;
			if (x > xb3max_p || xb3max_p == 530) xb3max_p <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		if (((xb3max_p - xb3min_p) > (xb2max_p - xb2min_p)) && ((xb3max_p - xb3min_p) > (xb1max_p - xb1min_p))) begin //b3 biggest
			x_max_p <= xb3max_p;
			if (xb3min_p - xb2max_p < 10) begin //condition when ball overlap 2nd boundary
				x_min_p <= xb2min_p;
			end
			else begin
				x_min_p <= xb3min_p;
			end
		end
		else if ((xb2max_p - xb2min_p > xb3max_p - xb3min_p) && (xb2max_p - xb2min_p > xb1max_p - xb1min_p) )begin //b2 biggest
			if (xb2min_p - xb1max_p < 10) begin // condition when ball overalap first boundary
				x_min_p <= xb1min_p;
			end
			else begin
				x_min_p <= xb2min_p;
			end
			
			if (xb3min_p - xb2max_p < 10) begin //condition when ball overlap second boundary
				x_max_p <= xb3max_p; 
			end
			else begin
				x_max_p <= xb2max_p;
			end
		end
		else if ((xb1max_p - xb1min_p > xb3max_p - xb3min_p) && (xb1max_p - xb1min_p > xb2max_p - xb2min_p)) begin //b1 biggest
			x_min_p <= xb1min_p;
			if (xb2min_p - xb1max_p < 10) begin //condition when ball overlap first boundary
				x_max_p <= xb2max_p;
			end
			else begin
				x_max_p <= xb1max_p;
			end
		end
		else begin
			x_max_p <= 0;
			x_min_p <= 0;
		end
	end
end

// GREEN BOX
always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_g <= IMAGE_W-11'h1;
		x_max_g <= 0;
		y_min_g <= IMAGE_H-11'h1;
		y_max_g <= 0;
		xb1max_g <= 0;
		xb1min_g <= 210;
		xb2max_g <= 210;
		xb2min_g <= 420;
		xb3max_g <= 420;
		xb3min_g <= 640;
	end
	
	if ((bound_green && in_valid && x<210)) begin	//Update bounds when the pixel is red
		if (x < xb1min_g && (xb1min_g - x < 5 || xb1min_g == 210)) xb1min_g <= x;
		if (x > xb1max_g && (x-xb1max_g < 5 || xb1max_g == 0)) xb1max_g <= x;
		//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
		//y_max_r <= y_min_r + (x_max_r - x_min_r);
	end
	else if ((bound_green && in_valid && x<420)) begin	//Update bounds when the pixel is red
		if (x < xb2min_g && (xb2min_g - x < 5 || xb2min_g == 420)) xb2min_g <= x;
		if (x > xb2max_g && (x-xb2max_g < 5 || xb2max_g == 210)) xb2max_g <= x;
		//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
		//y_max_r <= y_min_r + (x_max_r - x_min_r);
	end
	else if ((bound_green && in_valid && x<640)) begin	//Update bounds when the pixel is red
		if (x < xb3min_g && (xb3min_g - x < 5 || xb3min_g == 640)) xb3min_g <= x;
		if (x > xb3max_g && (x-xb3max_g < 5 || xb3max_g == 420)) xb3max_g <= x;
		//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
		//y_max_r <= y_min_r + (x_max_r - x_min_r);
	end
	
	if (xb3max_g - xb3min_g > xb2max_g - xb1min_g && xb3max_g - xb3min_g > xb1max_g - xb1min_g) begin
		x_max_g <= xb3max_g;
		if (xb3min_g == 420 && xb2max_g == 419) begin
			if (xb2min_g == 210) begin
				x_min_g <= xb1min_g;
			end
			else begin
				x_min_g <= xb2min_g;
			end
		end
		else begin
			x_min_g <= xb3min_g;
		end
	end
	
	else if (xb2max_g - xb2min_g > xb3max_g - xb3min_g && xb2max_g - xb2min_g > xb1max_g - xb1min_g) begin
		if (xb2min_g == 210 && xb1max_g == 219) begin
			x_min_g <= xb1min_g;
		end
		else begin
			x_min_g <= xb2min_g;
		end
		if (xb2max_g == 419) begin
			x_max_g <= xb3max_g; 
		end
		else begin
			x_max_g <= xb2max_g;
		end
	end
	
	else begin
		x_min_g <= xb1min_g;
		if (xb1max_g == 209 && xb2min_g == 210) begin
			if (xb2max_g == 419) begin
				x_max_g <= xb3max_g;
			end
			else begin
				x_max_g <= xb2max_g;
			end
		end
		else begin
			x_max_g <= xb1max_g;
		end
	end
end




//Process bounding box at the end of the frame.16 8 4 2 1
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

reg [2:0] color; //000 - none, 001 - red, 010 - blue, 011 - orange, 100 - pink, 101 - green
reg [2:0] pos;
reg [9:0] distance;
reg [15:0] msg;


always@(posedge clk) begin
	color <= 3'b000;
	pos <= 3'b010;
	distance <= 1023;
	
	if (red_detect) begin
		color <= 3'b001;
		distance <= x_max_r - x_min_r;
		pos <= x_min_r[8:6];
	end
	else if (blue_detect) begin
		color <= 3'b010;
		distance <= x_max_b - x_min_b;
		pos <= x_min_b[8:6];
	end
	else if (orange_detect) begin
		color <= 3'b011;
		distance <= x_max_o - x_min_o;
		pos <= x_min_o[8:6];
	end
	else if (pink_detect) begin
		color <= 3'b100;
		distance <= x_max_p - x_min_p;
		pos <= x_min_p[8:6];
	end
	if ((((struct2- struct1) - (struct3 - struct2) < 100 ) || ((struct2- struct1) - (struct3 - struct2) > -100 )) && (struct2 - struct1>150)) begin
		color <= 3'b111;
		distance <= struct2-struct1;
	end
	if ((((struct3- struct2) - (struct4 - struct3) < 100 ) || ((struct3- struct2) - (struct4 - struct3) > -100 )) && (struct3 - struct2 > 150)) begin
		color <= 3'b111;
		distance <= struct3-struct2;
	end
	if (mode==0) begin
		color <= 3'b110;
	end
	msg <= {distance,pos,color};
end

assign outbuffer = msg;

	
//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;
reg [31:0] distance_r, distance_y, distance_g, distance_b, distance_grey;
`define RED_BOX_MSG_ID "RBB"

wire[6:0] ratio1,ratio2;
assign ratio1 = 16'd79;
assign ratio2 = 16'd20;

wire [12:0] constan;
assign constan = 16'd7443;


//((732 * (79/20))/147) =19.66 ish 19
 // -> 14.9
reg [15:0] outt_r, outt_y;

//79/20 = 3.95 -> 3

always@(*) begin	//Write words to FIFO as state machine advances
	case(msg_state)
		2'b00: begin
			msg_buf_in = 32'd0; //Bottom right coordinate
			msg_buf_wr = 1'b0;
		end
		2'b01: begin
			msg_buf_in = `RED_BOX_MSG_ID;	//Message ID
			msg_buf_wr = 1'b1;
		end
		2'b10: begin
			//msg_buf_in = {5'b0, x_min, 5'b0, y_min};	//Top left coordinate
			outt_r = distance_y[15:0];
			outt_y = distance_r[15:0];
			/*for (i = 0; i < 16; i = i+1)begin
				out = out >> 1;
			end
			*/
			msg_buf_in = distance_g; //Bottom right coordinate
			msg_buf_wr = 1'b1; //changed!!!!!!!!!!
		end
		2'b11: begin
			//msg_buf_in = {5'b0, x_max, 5'b0, y_max};	//Top left coordinate
			msg_buf_in = distance_r; //Bottom right coordinate
			msg_buf_wr = 1'b1;  //REPLACED WITH DISTANCE
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
reg	[23:0]	bb_col;

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
						
reg	[23:0]	bb_col_r, bb_col_b, bb_col_o, bb_col_p , bb_col_g;

endmodule
