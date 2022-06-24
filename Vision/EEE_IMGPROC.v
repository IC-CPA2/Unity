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
parameter BB_COL_DEFAULT_CYAN   = 24'h00ffff;
parameter BB_COL_DEFAULT_YELLOW   = 24'hffff00;
parameter BB_COL_DEFAULT_DARK_GREEN   = 24'h195713; 
parameter BB_COL_DEFAULT_WHITE   = 24'hffffff;
parameter BB_COL_DEFAULT_BLACK   = 24'h6a0dad;


wire [7:0]   red, green, blue, grey;
wire [7:0]   red_out, green_out, blue_out;

wire         sop, eop, in_valid, out_ready;
////////////////////////////////////////////////////////////////////////
/// HSV conversion
wire [7:0] hue_init , hue_mid, min , max ,  sat, value, hue;

//assign max = (red >blue) ? ((red>green) ? red : green) : (blue > green) ? blue : green; // max(R,G,B)
//assign value = max;    // val = MAX
//assign min = (red < blue) ? ((red<green) ? red : green) : (blue < green) ? blue : green;
//assign sat = (value != 0) ? (value - min)* 255 / value : 0;
//// base hue 
//assign hue_init = (red == green && red == blue) ? 0 :((value != red)? (value != green) ? (((240*((value - min))+ (60* (red - green)))/(value-min))):
//                ((120*(value-min)+60*(blue - red))/(value - min)): 
//                (blue < green) ? ((60*(green - blue)/(value - min))): (((360*(value-min) +(60*(green - blue)))/(value - min))));
//assign hue_mid = (hue_init<0) ? hue_init + 360 : hue_init;
//	 
//assign hue = hue_mid>>1;





assign value = (red > green) ? ((red > blue) ? red[7:0] : blue[7:0]) : (green > blue) ? green[7:0] : blue[7:0];						
assign min = (red < green)? ((red<blue) ? red[7:0] : blue[7:0]) : (green < blue) ? green [7:0] : blue[7:0];
assign sat = (value - min)* 255 / value;
assign hue = (red == green && red == blue) ? 0 :((value != red)? (value != green) ? (((240*((value - min))+ (60* (red - green)))/(value-min))>>1):
                ((120*(value-min)+60*(blue - red))/(value - min)>>1): 
                (blue < green) ? ((60*(green - blue)/(value - min))>>1): (((360*(value-min) +(60*(green - blue)))/(value - min))>>1));





					 
/// Detect  areas
wire red_detect , orange_detect, pink_detect, blue_detect, any_detect, green_detect , dark_green_detect, black_detect, white_detect , yellow_detect;
 
 
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
reg prev_dg0, prev_dg1 , prev_dg2 ,prev_dg3, prev_dg4;  // last 5 dark green detects
reg prev_y0, prev_y1 , prev_y2 ,prev_y3, prev_y4;  // last 5 yellow detects
reg prev_bl0, prev_bl1 , prev_bl2 ,prev_bl3, prev_bl4, prev_bl5, prev_bl6 , prev_bl7 ,prev_bl8, prev_bl9, prev_bl10;  // last 5 black detects
reg prev_w0, prev_w1 , prev_w2 ,prev_w3, prev_w4, prev_w5, prev_w6 , prev_w7 ,prev_w8, prev_w9, prev_w10;  // last 5 white detects
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
	
	prev_y0 <=0;
	prev_y1 <=0;
	prev_y2 <=0;
	prev_y3 <=0;
	prev_y4 <=0;
	
	prev_y0 <=0;
	prev_y1 <=0;
	prev_y2 <=0;
	prev_y3 <=0;
	prev_y4 <=0;
	
	
	prev_dg0 <=0;
	prev_dg1 <=0;
	prev_dg2 <=0;
	prev_dg3 <=0;
	prev_dg4 <=0;
	
	
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
	
	
	prev_dg0 <=dark_green_detect;
	prev_dg1 <=prev_dg0;
	prev_dg2 <=prev_dg1;
	prev_dg3 <=prev_dg2;
	prev_dg4 <= prev_dg3;
	
	prev_y0 <=yellow_detect;
	prev_y1 <=prev_y0;
	prev_y2 <=prev_y1;
	prev_y3 <=prev_y2;
	prev_y4 <= prev_y3;
	
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
	
	prev_dg0 <=dark_green_detect;
	prev_dg1 <=prev_dg0;
	prev_dg2 <=prev_dg1;
	prev_dg3 <=prev_dg2;
	prev_dg4 <= prev_dg3;
	
	prev_bl0 <=black_detect;
	prev_bl1 <=prev_bl0;
	prev_bl2 <=prev_bl1;
	prev_bl3 <=prev_bl2;
	prev_bl4 <= prev_bl3;
	prev_bl5 <=prev_bl4;
	prev_bl6 <=prev_bl5;
	prev_bl7 <=prev_bl6;
	prev_bl8 <= prev_bl7;
	prev_bl9 <= prev_bl8;
	prev_bl10 <= prev_bl9;
	
	prev_w0 <=white_detect;
	prev_w1 <=prev_w0;
	prev_w2 <=prev_w1;
	prev_w3 <=prev_w2;
	prev_w4 <= prev_w3;
	prev_w5 <=prev_w4;
	prev_w6 <=prev_w5;
	prev_w7 <=prev_w6;
	prev_w8 <= prev_w7;
	prev_w9 <= prev_w8;
	prev_w10 <= prev_w9;
	
end
	
	
// COLOUR DETECTION USING MIXTURE OF HUES AND RGB CONDITIONS


wire rgb;
assign rgb = (red>green) && (green>blue);
//assign pink_detect = ((hue>=4 && hue <= 15) && (sat>=130 && sat <= 194) && (value>=249 && value <= 255));//(hue>=2 && hue <=16) && (sat>=117 && sat<=210) && (value>=200);//(((hue >= 0 && hue <= 15)||(hue >= 165 && hue <= 180)) && value > 120 && sat > 110);
//
////assign orange_detect  = ((hue>=20 && hue <= 30) && (sat>=107 && sat <= 228) && (value>=248 && value <= 255))
////||((hue >= 15 && hue <= 25 && value > 160 && sat > 175));//(hue >= 15 && hue <= 30 && value > 120 && sat > 110);
//
//assign blue_detect = ((hue>=80 && hue <= 118) && (sat>=40 && sat <= 153) && (value>=54 && value <= 144));
//
//
//
////assign blue_detect = ((hue>=80 && hue <= 120) && (sat>=53 && sat <= 192) && (value>=29 && value <=95))
////||((hue>=42 && hue <= 114) && (sat>=20 && sat <= 128) && (value>=24 && value <=41))
////||((hue>=73 && hue <= 125) && (sat>=49 && sat <= 182) && (value>=34 && value <= 128))
////||((hue>=80 && hue <= 120) && (sat>=53 && sat <= 189) && (value>=29 && value <= 83));
//
////assign blue_detect    = ((hue>=51 && hue <= 112) && (sat>=22 && sat <= 166) && (value>=31 && value <= 65))
////||((hue>=61 && hue <= 92) && (sat>=60 && sat <= 163) && (value>=46 && value <= 123))
////||((hue>=60 && hue <= 90) && (sat>=61 && sat <= 121) && (value>=119 && value <= 236));//((blue>green-20) && (blue>red) && ~pink_detect && ~orange_detect ) || (hue >= 55 && hue <= 85 && sat >= 51 && sat <= 89 && value >= 76 && value <= 240);
//
//assign   red_detect    = ((hue>=8 && hue <= 21) && (sat>=156 && sat <= 227) && (value>=208 && value <= 255))
//|| ((hue>=5 && hue <= 14) && (sat>=182 && sat <= 239) && (value>=115 && value <= 255))
//||((hue>=5 && hue <= 13) && (sat>=187 && sat <= 254) && (value>=83 && value <= 180))
//|| ((hue>=6 && hue <= 15) && (sat>=178 && sat <= 234) && (value>=200 && value <= 255))
//|| ((hue>=6 && hue <=18) && (sat>=190 && sat<= 250) && (value>=130 && value<=240)); //(rgb && (red>green+30) && (red>blue+30) && ~pink_detect && ~orange_detect);//(((hue >= 0 && hue <= 15)||(hue >= 165 && hue <= 180)) && value > 120 && sat > 110);
//
//assign 	green_detect = ((hue>=52 && hue <= 68) && (sat>=80 && sat <= 128) && (value>=122 && value <= 160))
//||((hue>=50 && hue <= 61) && (sat>=155 && sat <= 206) && (value>=101 && value <= 191))
//||((hue>=44 && hue <= 59) && (sat>=122 && sat <= 200) && (value>=159 && value <= 255))
//||((hue>=44 && hue <= 59) && (sat>=122 && sat <= 200) && (value>=159 && value <= 255));//((green>blue+10) && (green>red+30) && (45 <= hue & hue <= 65));
//
////assign green_detect = ((hue>=49 && hue <= 56) && (sat>=166 && sat <= 210) && (value>=120 && value <= 255));
//
//
//assign 	dark_green_detect = ((hue>=50 && hue <= 71) && (sat>=81 && sat <= 156) && (value>=58 && value <= 139))
//||((hue>=76 && hue <= 105) && (sat>=83 && sat <= 151) && (value>=62 && value <= 158))
//||((hue>=54 && hue <= 77) && (sat>=90 && sat <= 202) && (value>=60 && value <= 191));
////assign 	dark_green_detect = ((hue>=52 && hue <= 82) && (sat>=86 && sat <= 185) && (value>=30 && value <= 138))
////||((hue>=45 && hue <= 74) && (sat>=78 && sat <= 165) && (value>=47 && value <= 126))
////||((hue>=54 && hue <= 68) && (sat>=88 && sat <= 136) && (value>=63 && value <= 135));
//
//
//assign 	yellow_detect = ((hue>=23 && hue <= 32) && (sat>=184 && sat <= 232) && (value>=141 && value <= 254))
//||((hue>=27 && hue <= 33) && (sat>=180 && sat <= 230) && (value>=126 && value <= 252))
//||((hue>=29 && hue <= 36) && (sat>=177 && sat <= 212) && (value>=151 && value <= 228))
//||((hue>=20 && hue <= 30) && (sat>=107 && sat <= 228) && (value>=248 && value <= 255))
//||((hue >= 15 && hue <= 25 && value > 160 && sat > 175));// curerntly using orange(hue >= 15 && hue <= 30 && value > 120 && sat > 110) //((hue>=23 && hue <= 31) && (sat>=189 && sat <= 235) && (value>=140 && value <= 255));
//
assign black_detect = (((hue>=0 && hue <= 156) && (sat>=0 && sat <= 92) && (value>=47 && value <= 70))
||((hue>=0 && hue <= 130) && (sat>=0 && sat <= 127) && (value>=30 && value <= 50))
||((hue>=107 && hue <= 153) && (sat>=57 && sat <= 135) && (value>=41 && value <= 54)))
 && in_range_struct;
//(((red <= 78 && green <= 78 && blue <= 78) && (green - red < 20) && (green - blue < 20))) && in_range_struct;
//
assign   any_detect = ((pink_detect || orange_detect || blue_detect || red_detect || green_detect ));
//
assign white_detect = (((hue>=70 && hue <= 141) && (sat>=5 && sat <= 77) && (value>=93 && value <= 136))
||((hue>=0 && hue <= 172) && (sat>=0 && sat <= 41) && (value>=115 && value <= 171))
||((hue>=0 && hue <= 120) && (sat>=0 && sat <= 31) && (value>=184 && value <= 213)) 
||((hue>=70 && hue <= 110) && (sat>=2 && sat <= 11) && (value>=253 && value <= 255)) 
|| ((red>=158 && red <= 195) && (blue>=162 && blue <= 201) && (green>=149 && green <= 197))
|| ((red>=217 && red <= 255) && (blue>=236 && blue <= 255) && (green>=233 && green <= 255))) && in_range_struct;

assign pink_detect = ((hue>=159 && hue <= 177) && (sat>=51 && sat <= 159) && (value>=185 && value <= 255));

assign red_detect = ((hue>=178 || hue <= 5) && (sat>=125 && sat <= 184) && (value>=145 && value <= 255));

assign yellow_detect = ((hue>=25 && hue <= 37) && (sat>=98 && sat <= 146) && (value>=132 && value <= 255));

assign green_detect = ((hue>=67 && hue <= 90) && (sat>=75 && sat <= 165) && (value>=149 && value <= 255));

assign blue_detect = ((hue>=104 && hue <= 120) && (sat>=121 && sat <= 212) && (value>=48 && value <= 184))
||((hue>=106 && hue <= 118) && (sat>=134 && sat <= 204) && (value>=61 && value <= 178))
||((hue>=105 && hue <= 121) && (sat>=130 && sat <= 212) && (value>=54 && value <= 163))
||((hue>=107 && hue <= 121) && (sat>=145 && sat <= 206) && (value>=58 && value <= 132));

assign dark_green_detect = ((hue>=82 && hue <= 107) && (sat>=104 && sat <= 199) && (value>=45 && value <= 159))
||((hue>=90 && hue <= 106) && (sat>=100 && sat <= 182) && (value>=57 && value <= 133));

wire new;
assign new = white_detect || black_detect;

// Find boundary of cursor box
wire in_range_struct;
assign in_range_struct = (y  > 300) && (y<370) && (x>50) && (x<600);

wire in_range;
assign in_range = (y>200);


wire in_range_blue;
assign in_range_blue = (x>50) && (x<600);
wire want_black;
assign want_black = 1;

// Highlight detected areas
wire [23:0] red_high;
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign red_high  =  (red_detect && prev_r0 && prev_r1 && prev_r2 && in_range) ? {8'hff,8'h0,8'h0} 
	: ((blue_detect && prev_b0 && prev_b1 && prev_b2 && prev_b3 && prev_b4 && in_range && in_range_blue)? {8'h0,8'h0,8'hff} 
	: ((orange_detect && prev_o0 && prev_o1 && prev_o2 && prev_o3 && prev_o4 && in_range)? {8'hde, 8'h9a, 8'h7}
	: ((pink_detect && prev_p0 && prev_p1 && prev_p2 && prev_p3 && prev_p4 && in_range)? {8'hff, 8'hba, 8'hfd}
	: ((green_detect && prev_g0 && prev_g1 && prev_g2 && prev_g3 && prev_g4 && in_range)? {8'h00, 8'hff, 8'h00}
	: ((dark_green_detect && prev_dg0 && prev_dg1  && in_range)? {8'h19, 8'h57, 8'h13}
	: ((yellow_detect && prev_y0 && prev_y1 && prev_y2 && prev_y3 && prev_y4 && in_range)? {8'hff, 8'hff, 8'h00}
	: ((black_detect && prev_bl0 && prev_bl1 && prev_bl2  && want_black)? {8'h6a, 8'h0d, 8'had}
	: ((white_detect && prev_w0 && prev_w1  && prev_w2 && prev_w3 )? {8'h87, 8'hce, 8'heb}
	: {grey,grey,grey}))))))));

// light blue is white and purple is black
// Show bounding box RED
wire [23:0] new_image; //, new_image_r, new_image_b, new_image,p, new_immage_o;
wire bb_active_bl , bb_active_ww, bb_active_r , bb_active_b, bb_active_p , bb_active_n, bb_active_o , bb_active_g, struct_active , bb_active_dg, bb_active_y , bb_active_w;
assign bb_active_r = (x == left_r) | (x == right_r) | (y == top_r) | (y == bottom_r);
assign bb_active_p = (x == left_p) | (x == right_p) | (y == top_p) | (y == bottom_p);
assign bb_active_b = (x == left_b) | (x == right_b) | (y == top_b) | (y == bottom_b);
assign bb_active_o = (x == left_o) | (x == right_o) | (y == top_o) | (y == bottom_o);
assign bb_active_g = (x == left_g) | (x == right_g) | (y == top_g) | (y == bottom_g);
assign bb_active_dg = (x == left_dg) | (x == right_dg) | (y == top_dg) | (y == bottom_dg);
assign bb_active_y = (x == left_y) | (x == right_y) | (y == top_y) | (y == bottom_y);
assign bb_active_w = (x == left_w) | (x == right_w) | (y == top_w) | (y == bottom_w);
assign bb_active_bl = (x == left_bl) | (x == right_bl) | (y == top_bl) | (y == bottom_bl);
assign bb_active_n = (x == left_n) | (x == right_n) | (y == top_n) | (y == bottom_n);
assign bb_active_ww = (x == left_ww) | (x == right_ww) | (y == top_ww) | (y == bottom_ww);
//assign bb_active_black = (x == left_bl) | (x == right_bl);
//assign struct_active = (x == struct1) | (x == struct2) | (x == struct3) | (x == struct4);

//assign new_image_r = bb_active_r ? bb_col_r : red_high;








assign new_image  =  (bb_active_r && 1) ? bb_col_r  
	: ((bb_active_p && 1)? bb_col_p
	: ((bb_active_b && 1)? bb_col_b
	: ((struct_bound && 0)? bb_col_cyan
	: ((bb_active_dg && 1)? bb_col_dg
	: ((bb_active_g && 1)? bb_col_g
	: ((bb_active_y && 1)? bb_col_y
	: ((bb_active_w && 1)? bb_col_w
	: ((bb_active_bl && 1)? bb_col_bl
	: ((bb_active_n && 0)? bb_col_g
	: ((bb_active_ww && 1)? bb_col_o
	: ((sb && (y==260) && 0)? bb_col_r
	: red_high)))))))))));

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
assign prev10blckdetect = (prev_bl10 && prev_bl9) || (prev_bl9 && prev_bl8)  ;//(prev_red10 <= 78 && prev_green10 <= 78 && prev_blue10 <= 78) && (prev_green10 - prev_red10 < 20) && (prev_green10 - prev_blue10 < 20);
assign prev10whtedetect = (prev_w10 && prev_w9) || (prev_w9 && prev_w8);//(prev_red10 >= 60 && prev_green10 >= 80 && prev_blue10 >= 60) && (prev_green10 - prev_red10 < 20) && (prev_green10 - prev_blue10 < 20);
assign blckdetect = black_detect; //(red <= 78 && green <= 78 && blue <= 78) && (green - red < 20) && (green - blue < 20);
assign whtedetect = white_detect; // || (red >= 60 && green >= 80 && blue >= 60) && (green - red < 20) && (green - blue < 20);

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
		if(struct_detect && (y==270)) begin
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

reg [15:0] c_sum;
reg [15:0] p_sum;
reg [15:0] d_sum;
reg sb;
reg [15:0] s1;

always@(posedge clk) begin
 if (y==240) begin
	c_sum <=  red+green+blue;
	p_sum <= prev_red10 + prev_green10 + prev_blue10;
	if (c_sum > p_sum) begin
		d_sum = c_sum - p_sum;
	end
	else begin;
		d_sum = p_sum - c_sum;
	end 
	if (d_sum > 500) begin
		sb <=1;
	end
	else begin
		sb<= 0;
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
reg [10:0] xb1max_dg, xb1min_dg, xb2max_dg, xb2min_dg, xb3max_dg, xb3min_dg; 
reg [10:0] xb1max_y, xb1min_y, xb2max_y, xb2min_y, xb3max_y, xb3min_y; 
reg [10:0] xb1max_w, xb1min_w, xb2max_w, xb2min_w, xb3max_w, xb3min_w;
reg [10:0] xb1max_bl, xb1min_bl, xb2max_bl, xb2min_bl, xb3max_bl, xb3min_bl;
reg [10:0] xb1max_n, xb1min_n, xb2max_n, xb2min_n, xb3max_n, xb3min_n;



reg [10:0] x_min_o, y_min_o, x_max_o, y_max_o;
reg [10:0] x_min_b, y_min_b, x_max_b, y_max_b;
reg [10:0] x_min_p, y_min_p, x_max_p, y_max_p;
reg [10:0] x_min_bl, y_min_bl, x_max_bl, y_max_bl;
reg [10:0] x_min_w, y_min_w, x_max_w, y_max_w;
reg [10:0] x_min_g, y_min_g, x_max_g, y_max_g;
reg [10:0] x_min_n, y_min_n, x_max_n, y_max_n;
reg [10:0] x_min_dg, y_min_dg, x_max_dg, y_max_dg;
reg [10:0] x_min_y, y_min_y, x_max_y, y_max_y;
reg [10:0] x_min, y_min, x_max, y_max; // THIS IS JUST THERE AS IT IS USED IN THE MESSAGE FUNCTIONS
wire  bound_orange , bound_ww,bound_new, bound_pink, bound_red, bound_blue, bound_green , bound_yellow, bound_black, bound_dark_green, bound_white;
assign bound_orange = orange_detect && prev_o0 && prev_o1 && prev_o2 && prev_o3  && prev_o4;
assign bound_red = red_detect && prev_r0 && prev_r1 && prev_r2 && prev_r3  && prev_r4;
assign bound_blue = blue_detect && prev_b0 && prev_b1 && prev_b2 && prev_b3;
assign bound_pink = pink_detect && prev_p0 && prev_p1 && prev_p2 && prev_p3  && prev_p4;
assign bound_green = green_detect && prev_g0 && prev_g1 && prev_g2 && prev_g3  && prev_g4;  
assign bound_yellow = yellow_detect && prev_y0 && prev_y1 && prev_y2 && prev_y3  && prev_y4;  
assign bound_white = white_detect && prev_w0 && prev_w1 && prev_w2 && prev_w3  && prev_w4; 
assign bound_dark_green = dark_green_detect && prev_dg0 ;
assign bound_black = black_detect && prev_bl0 && prev_bl1 && prev_bl2 && prev_bl3  && prev_bl4;
assign bound_new = new && (prev_bl0||prev_w0) && (prev_bl1||prev_w1) && (prev_bl2||prev_w2);



// White naive bb

reg [10:0] x_min_ww, y_min_ww, x_max_ww, y_max_ww;
always@(posedge clk) begin
	if (white_detect & in_valid) begin	//Update bounds when the pixel is red
		if (x < x_min_ww) x_min_ww <= x;
		if (x > x_max_ww) x_max_ww <= x;
		if (y < y_min_ww) y_min_ww <= y;
		y_max_ww <= y;
	end
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_ww <= IMAGE_W-11'h1;
		x_max_ww <= 0;
		y_min_ww <= IMAGE_H-11'h1;
		y_max_ww <= 0;
	end
end









// NEW BOX
always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_n <= IMAGE_W-11'h1;
		x_max_n <= 0;
		y_min_n <= IMAGE_H-11'h1;
		y_max_n <= 0;
		xb1max_n <= 105;
		xb1min_n <= 104;
		xb2max_n <= 315;
		xb2min_n <= 314;
		xb3max_n <= 530;
		xb3min_n <= 529;
	end
	else begin
		if ((bound_new && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_n || xb1min_n == 104) xb1min_n <= x;
			if (x > xb1max_n || xb1max_n == 105) xb1max_n <= x;
			//if (y < y_min_n && (y_min_n - y <5||y_min_n == IMAGE_H-11'h1)) y_min_n <= y;
			//y_max_n <= y_min_n + (x_max_n - x_min_n);
		end
		else if ((bound_new && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_n || xb2min_n == 314) xb2min_n <= x;
			if (x > xb2max_n || xb2max_n == 315) xb2max_n <= x;
			//if (y < y_min_n && (y_min_n - y <5||y_min_n == IMAGE_H-11'h1)) y_min_n <= y;
			//y_max_n <= y_min_n + (x_max_n - x_min_n);
		end
		else if ((bound_new && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_n || xb3min_n == 529) xb3min_n <= x;
			if (x > xb3max_n || xb3max_n == 530) xb3max_n <= x;
			//if (y < y_min_n && (y_min_n - y <5||y_min_n == IMAGE_H-11'h1)) y_min_n <= y;
			//y_max_n <= y_min_n + (x_max_n - x_min_n);
		end
		if (((xb3max_n - xb3min_n) > (xb2max_n - xb2min_n)) && ((xb3max_n - xb3min_n) > (xb1max_n - xb1min_n))) begin //b3 biggest
			x_max_n <= xb3max_n;
			if (xb3min_n - xb2max_n < 10) begin //condition when ball overlap 2nd boundary
				x_min_n <= xb2min_n;
			end
			else begin
				x_min_n <= xb3min_n;
			end
		end
		else if ((xb2max_n - xb2min_n > xb3max_n - xb3min_n) && (xb2max_n - xb2min_n > xb1max_n - xb1min_n))begin //b2 biggest
			if (xb2min_n - xb1max_n < 10) begin // condition when ball overalap first boundary
				x_min_n <= xb1min_n;
			end
			else begin
				x_min_n <= xb2min_n;
			end
			
			if (xb3min_n - xb2max_n < 10) begin //condition when ball overlap second boundary
				x_max_n <= xb3max_n; 
			end
			else begin
				x_max_n <= xb2max_n;
			end
		end
		else if ((xb1max_n - xb1min_n > xb3max_n - xb3min_n) && (xb1max_n- xb1min_n > xb2max_n - xb2min_n)) begin //b1 biggest
			x_min_n <= xb1min_n;
			if (xb2min_n - xb1max_n < 10) begin //condition when ball overlap first boundary
				x_max_n <= xb2max_n;
			end
			else begin
				x_max_n <= xb1max_n;
			end
		end
		else begin
			x_max_n <= 0;
			x_min_n <= 0;
		end
	end
end
 
 
 
 
//BLUE BOX 
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
		else if ((xb1max_b - xb1min_b > xb3max_b - xb3min_b) && (xb1max_b- xb1min_b > xb2max_b - xb2min_b)) begin //b1 biggest
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

wire naive;
assign naive = 1;




//BLACK BOX 
always@(posedge clk) begin
	if (naive) begin
		if (black_detect & in_valid) begin	//Update bounds when the pixel is red
			if (x < x_min_bl) x_min_bl <= x;
			if (x > x_max_bl) x_max_bl <= x;
			if (y < y_min_bl) y_min_bl <= y;
			y_max_bl <= y;
		end
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_bl <= IMAGE_W-11'h1;
		x_max_bl <= 0;
		y_min_bl <= IMAGE_H-11'h1;
		y_max_bl <= 0;
	end
	end
	
	
	else begin
		if (sop & in_valid) begin	//Reset bounds on start of packet
			x_min_bl <= IMAGE_W-11'h1;
			x_max_bl <= 0;
			y_min_bl <= IMAGE_H-11'h1;
			y_max_bl <= 0;
			xb1max_bl <= 105;
			xb1min_bl <= 104;
			xb2max_bl <= 315;
			xb2min_bl <= 314;
			xb3max_bl <= 530;
			xb3min_bl <= 529;
		end
		else begin
			if ((bound_black && in_valid && x<210)) begin	//Update bounds when the pixel is red
				if (x < xb1min_bl || xb1min_bl == 104) xb1min_bl <= x;
				if (x > xb1max_bl || xb1max_bl == 105) xb1max_bl <= x;
				//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
				//y_max_r <= y_min_r + (x_max_r - x_min_r);
			end
			else if ((bound_black && in_valid && x<420)) begin	//Update bounds when the pixel is red
				if (x < xb2min_bl || xb2min_bl == 314) xb2min_bl <= x;
				if (x > xb2max_bl || xb2max_bl == 315) xb2max_bl <= x;
				//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
				//y_max_r <= y_min_r + (x_max_r - x_min_r);
			end
			else if ((bound_black && in_valid)) begin	//Update bounds when the pixel is red
				if (x < xb3min_bl || xb3min_bl == 529) xb3min_bl <= x;
				if (x > xb3max_bl || xb3max_bl == 530) xb3max_bl <= x;
				//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
				//y_max_r <= y_min_r + (x_max_r - x_min_r);
			end
			if (((xb3max_bl - xb3min_bl) > (xb2max_bl - xb2min_bl)) && ((xb3max_bl - xb3min_bl) > (xb1max_bl - xb1min_bl))) begin //b3 biggest
				x_max_bl <= xb3max_bl;
				if (xb3min_bl - xb2max_bl < 10) begin //condition when ball overlap 2nd boundary
					x_min_bl <= xb2min_bl;
				end
				else begin
					x_min_bl <= xb3min_bl;
				end
			end
			else if ((xb2max_bl - xb2min_bl > xb3max_bl - xb3min_bl) && (xb2max_bl - xb2min_bl > xb1max_bl - xb1min_bl))begin //b2 biggest
				if (xb2min_bl - xb1max_bl < 10) begin // condition when ball overalap first boundary
					x_min_bl <= xb1min_bl;
				end
				else begin
					x_min_bl <= xb2min_bl;
				end
				
				if (xb3min_bl - xb2max_bl < 10) begin //condition when ball overlap second boundary
					x_max_bl <= xb3max_bl; 
				end
				else begin
					x_max_bl <= xb2max_bl;
				end
			end
			else if ((xb1max_bl - xb1min_bl > xb3max_bl - xb3min_bl) && (xb1max_bl- xb1min_bl > xb2max_bl - xb2min_bl)) begin //b1 biggest
				x_min_bl <= xb1min_bl;
				if (xb2min_bl - xb1max_bl < 10) begin //condition when ball overlap first boundary
					x_max_bl <= xb2max_bl;
				end
				else begin
					x_max_bl <= xb1max_bl;
				end
			end
			else begin
				x_max_bl <= 0;
				x_min_bl <= 0;
			end
		end
	end
end




//always@(posedge clk) begin
//	if (blue_detect && in_valid) begin	//Update bounds when the pixel is red
//		if (x < x_min_b) x_min_b <= x;
//		if (x > x_max_b) x_max_b <= x;
//		if (y < y_min_b) y_min_b <= y;
//		y_max_b <= y;
//	end
//	if (sop & in_valid) begin	//Reset bounds on start of packet
//		x_min_b <= IMAGE_W-11'h1;
//		x_max_b <= 0;
//		y_min_b <= IMAGE_H-11'h1;
//		y_max_b <= 0;
//	end
//end
  
//WHITE BOX 
always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_w <= IMAGE_W-11'h1;
		x_max_w <= 0;
		y_min_w <= IMAGE_H-11'h1;
		y_max_w <= 0;
		xb1max_w <= 105;
		xb1min_w <= 104;
		xb2max_w <= 315;
		xb2min_w <= 314;
		xb3max_w <= 530;
		xb3min_w <= 529;
	end
	else begin
		if ((bound_white && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_w || xb1min_w == 104) xb1min_w <= x;
			if (x > xb1max_w || xb1max_w == 105) xb1max_w <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_white && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_w || xb2min_w == 314) xb2min_w <= x;
			if (x > xb2max_w || xb2max_w == 315) xb2max_w <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		else if ((bound_white && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_w || xb3min_w == 529) xb3min_w <= x;
			if (x > xb3max_w || xb3max_w == 530) xb3max_w <= x;
			//if (y < y_min_r && (y_min_r - y <5||y_min_r == IMAGE_H-11'h1)) y_min_r <= y;
			//y_max_r <= y_min_r + (x_max_r - x_min_r);
		end
		if (((xb3max_w - xb3min_w) > (xb2max_w - xb2min_w)) && ((xb3max_w - xb3min_w) > (xb1max_w - xb1min_w))) begin //b3 biggest
			x_max_w <= xb3max_w;
			if (xb3min_w - xb2max_w < 10) begin //condition when ball overlap 2nd boundary
				x_min_w <= xb2min_w;
			end
			else begin
				x_min_w <= xb3min_w;
			end
		end
		else if ((xb2max_w - xb2min_w > xb3max_w - xb3min_w) && (xb2max_w - xb2min_w > xb1max_w - xb1min_w))begin //b2 biggest
			if (xb2min_w - xb1max_w < 10) begin // condition when ball overalap first boundary
				x_min_w <= xb1min_w;
			end
			else begin
				x_min_w <= xb2min_w;
			end
			
			if (xb3min_w - xb2max_w < 10) begin //condition when ball overlap second boundary
				x_max_w <= xb3max_w; 
			end
			else begin
				x_max_w <= xb2max_w;
			end
		end
		else if ((xb1max_w - xb1min_w > xb3max_w - xb3min_w) && (xb1max_w- xb1min_w > xb2max_w - xb2min_w)) begin //b1 biggest
			x_min_w <= xb1min_w;
			if (xb2min_w - xb1max_w < 10) begin //condition when ball overlap first boundary
				x_max_w <= xb2max_w;
			end
			else begin
				x_max_w <= xb1max_w;
			end
		end
		else begin
			x_max_w <= 0;
			x_min_w <= 0;
		end
	end
end
  
  
//RED BOX 
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

//GREEN BOX 
always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_g <= IMAGE_W-11'h1;
		x_max_g <= 0;
		y_min_g <= IMAGE_H-11'h1;
		y_max_g <= 0;
		xb1max_g <= 105;
		xb1min_g <= 104;
		xb2max_g <= 315;
		xb2min_g <= 314;
		xb3max_g <= 530;
		xb3min_g <= 529;
	end
	else begin
		if ((bound_green && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_g || xb1min_g == 104) xb1min_g <= x;
			if (x > xb1max_g || xb1max_g == 105) xb1max_g <= x;
			//if (y < y_min_g && (y_min_g - y <5||y_min_g == IMAGE_H-11'h1)) y_min_g <= y;
			//y_max_g <= y_min_g + (x_max_g - x_min_g);
		end
		else if ((bound_green && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_g || xb2min_g == 314) xb2min_g <= x;
			if (x > xb2max_g || xb2max_g == 315) xb2max_g <= x;
			//if (y < y_min_g && (y_min_g - y <5||y_min_g == IMAGE_H-11'h1)) y_min_g <= y;
			//y_max_g <= y_min_g + (x_max_g - x_min_g);
		end
		else if ((bound_green && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_g || xb3min_g == 529) xb3min_g <= x;
			if (x > xb3max_g || xb3max_g == 530) xb3max_g <= x;
			//if (y < y_min_g && (y_min_g - y <5||y_min_g == IMAGE_H-11'h1)) y_min_g <= y;
			//y_max_g <= y_min_g + (x_max_g - x_min_g);
		end
		if (((xb3max_g - xb3min_g) > (xb2max_g - xb2min_g)) && ((xb3max_g - xb3min_g) > (xb1max_g - xb1min_g))) begin //b3 biggest
			x_max_g <= xb3max_g;
			if (xb3min_g - xb2max_g < 10) begin //condition when ball overlap 2nd boundary
				x_min_g <= xb2min_g;
			end
			else begin
				x_min_g <= xb3min_g;
			end
		end
		else if ((xb2max_g - xb2min_g > xb3max_g - xb3min_g) && (xb2max_g - xb2min_g > xb1max_g - xb1min_g))begin //b2 biggest
			if (xb2min_g - xb1max_g < 10) begin // condition when ball overalap first boundary
				x_min_g <= xb1min_g;
			end
			else begin
				x_min_g <= xb2min_g;
			end
			
			if (xb3min_g - xb2max_g < 10) begin //condition when ball overlap second boundary
				x_max_g <= xb3max_g; 
			end
			else begin
				x_max_g <= xb2max_g;
			end
		end
		else if ((xb1max_g - xb1min_g > xb3max_g - xb3min_g) && (xb1max_g- xb1min_g > xb2max_g - xb2min_g)) begin //b1 biggest
			x_min_g <= xb1min_g;
			if (xb2min_g - xb1max_g < 10) begin //condition when ball overlap first boundary
				x_max_g <= xb2max_g;
			end
			else begin
				x_max_g <= xb1max_g;
			end
		end
		else begin
			x_max_g <= 0;
			x_min_g <= 0;
		end
	end
end



//DARK GREEN BOX 
always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_dg <= IMAGE_W-11'h1;
		x_max_dg <= 0;
		y_min_dg <= IMAGE_H-11'h1;
		y_max_dg <= 0;
		xb1max_dg <= 105;
		xb1min_dg <= 104;
		xb2max_dg <= 315;
		xb2min_dg <= 314;
		xb3max_dg <= 530;
		xb3min_dg <= 529;
	end
	else begin
		if ((bound_dark_green && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_dg || xb1min_dg == 104) xb1min_dg <= x;
			if (x > xb1max_dg || xb1max_dg == 105) xb1max_dg <= x;
			//if (y < y_min_dg && (y_min_dg - y <5||y_min_dg == IMAGE_H-11'h1)) y_min_dg <= y;
			//y_max_dg <= y_min_dg + (x_max_dg - x_min_dg);
		end
		else if ((bound_dark_green && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_dg || xb2min_dg == 314) xb2min_dg <= x;
			if (x > xb2max_dg || xb2max_dg == 315) xb2max_dg <= x;
			//if (y < y_min_dg && (y_min_dg - y <5||y_min_dg == IMAGE_H-11'h1)) y_min_dg <= y;
			//y_max_dg <= y_min_dg + (x_max_dg - x_min_dg);
		end
		else if ((bound_dark_green && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_dg || xb3min_dg == 529) xb3min_dg <= x;
			if (x > xb3max_dg || xb3max_dg == 530) xb3max_dg <= x;
			//if (y < y_min_dg && (y_min_dg - y <5||y_min_dg == IMAGE_H-11'h1)) y_min_dg <= y;
			//y_max_dg <= y_min_dg + (x_max_dg - x_min_dg);
		end
		if (((xb3max_dg - xb3min_dg) > (xb2max_dg - xb2min_dg)) && ((xb3max_dg - xb3min_dg) > (xb1max_dg - xb1min_dg))) begin //b3 biggest
			x_max_dg <= xb3max_dg;
			if (xb3min_dg - xb2max_dg < 10) begin //condition when ball overlap 2nd boundary
				x_min_dg <= xb2min_dg;
			end
			else begin
				x_min_dg <= xb3min_dg;
			end
		end
		else if ((xb2max_dg - xb2min_dg > xb3max_dg - xb3min_dg) && (xb2max_dg - xb2min_dg > xb1max_dg - xb1min_dg))begin //b2 biggest
			if (xb2min_dg - xb1max_dg < 10) begin // condition when ball overalap first boundary
				x_min_dg <= xb1min_dg;
			end
			else begin
				x_min_dg <= xb2min_dg;
			end
			
			if (xb3min_dg - xb2max_dg < 10) begin //condition when ball overlap second boundary
				x_max_dg <= xb3max_dg; 
			end
			else begin
				x_max_dg <= xb2max_dg;
			end
		end
		else if ((xb1max_dg - xb1min_dg > xb3max_dg - xb3min_dg) && (xb1max_dg- xb1min_dg > xb2max_dg - xb2min_dg)) begin //b1 biggest
			x_min_dg <= xb1min_dg;
			if (xb2min_dg - xb1max_dg < 10) begin //condition when ball overlap first boundary
				x_max_dg <= xb2max_dg;
			end
			else begin
				x_max_dg <= xb1max_dg;
			end
		end
		else begin
			x_max_dg <= 0;
			x_min_dg <= 0;
		end
	end
end


//YELLOW BOX 
always@(posedge clk) begin
	if (sop & in_valid) begin	//Reset bounds on start of packet
		x_min_y <= IMAGE_W-11'h1;
		x_max_y <= 0;
		y_min_y <= IMAGE_H-11'h1;
		y_max_y <= 0;
		xb1max_y <= 105;
		xb1min_y <= 104;
		xb2max_y <= 315;
		xb2min_y <= 314;
		xb3max_y <= 530;
		xb3min_y <= 529;
	end
	else begin
		if ((bound_yellow && in_valid && x<210)) begin	//Update bounds when the pixel is red
			if (x < xb1min_y || xb1min_y == 104) xb1min_y <= x;
			if (x > xb1max_y || xb1max_y == 105) xb1max_y <= x;
			//if (y < y_min_y && (y_min_y - y <5||y_min_y == IMAGE_H-11'h1)) y_min_y <= y;
			//y_max_y <= y_min_y + (x_max_y - x_min_y);
		end
		else if ((bound_yellow && in_valid && x<420)) begin	//Update bounds when the pixel is red
			if (x < xb2min_y || xb2min_y == 314) xb2min_y <= x;
			if (x > xb2max_y || xb2max_y == 315) xb2max_y <= x;
			//if (y < y_min_y && (y_min_y - y <5||y_min_y == IMAGE_H-11'h1)) y_min_y <= y;
			//y_max_y <= y_min_y + (x_max_y - x_min_y);
		end
		else if ((bound_yellow && in_valid)) begin	//Update bounds when the pixel is red
			if (x < xb3min_y || xb3min_y == 529) xb3min_y <= x;
			if (x > xb3max_y || xb3max_y == 530) xb3max_y <= x;
			//if (y < y_min_y && (y_min_y - y <5||y_min_y == IMAGE_H-11'h1)) y_min_y <= y;
			//y_max_y <= y_min_y + (x_max_y - x_min_y);
		end
		if (((xb3max_y - xb3min_y) > (xb2max_y - xb2min_y)) && ((xb3max_y - xb3min_y) > (xb1max_y - xb1min_y))) begin //b3 biggest
			x_max_y <= xb3max_y;
			if (xb3min_y - xb2max_y < 10) begin //condition when ball overlap 2nd boundary
				x_min_y <= xb2min_y;
			end
			else begin
				x_min_y <= xb3min_y;
			end
		end
		else if ((xb2max_y - xb2min_y > xb3max_y - xb3min_y) && (xb2max_y - xb2min_y > xb1max_y - xb1min_y))begin //b2 biggest
			if (xb2min_y - xb1max_y < 10) begin // condition when ball overalap first boundary
				x_min_y <= xb1min_y;
			end
			else begin
				x_min_y <= xb2min_y;
			end
			
			if (xb3min_y - xb2max_y < 10) begin //condition when ball overlap second boundary
				x_max_y <= xb3max_y; 
			end
			else begin
				x_max_y <= xb2max_y;
			end
		end
		else if ((xb1max_y - xb1min_y > xb3max_y - xb3min_y) && (xb1max_y- xb1min_y > xb2max_y - xb2min_y)) begin //b1 biggest
			x_min_y <= xb1min_y;
			if (xb2min_y - xb1max_y < 10) begin //condition when ball overlap first boundary
				x_max_y <= xb2max_y;
			end
			else begin
				x_max_y <= xb1max_y;
			end
		end
		else begin
			x_max_y <= 0;
			x_min_y <= 0;
		end
	end
end



//Process bounding box at the end of the frame.16 8 4 2 1
reg [1:0] msg_state;
reg [10:0] left_r, right_r, top_r, bottom_r;
reg [10:0] left_n, right_n, top_n, bottom_n;
reg [10:0] left_b, right_b, top_b, bottom_b;
reg [10:0] left_w, right_w, top_w, bottom_w;
reg [10:0] left_o, right_o, top_o, bottom_o;
reg [10:0] left_p, right_p, top_p, bottom_p;
reg [10:0] left_g, right_g, top_g, bottom_g;
reg [10:0] left_dg, right_dg, top_dg, bottom_dg;
reg [10:0] left_y, right_y, top_y, bottom_y;
reg [10:0] left_bl, right_bl , top_bl , bottom_bl;
reg [10:0] left_ww, right_ww, top_ww, bottom_ww;
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
		
		//Latch edges for display overlay on next frame  (DARK GREEN BOX)
		left_dg <= x_min_dg;
		right_dg <= x_max_dg;
		top_dg <= y_min_dg;
		bottom_dg <= y_max_dg;
		
		//Latch edges for display overlay on next frame  (YELLOW BOX)
		left_y <= x_min_y;
		right_y <= x_max_y;
		top_y <= y_min_y;
		bottom_y <= y_max_y;
		
		//Latch edges for display overlay on next frame  (WHITE BOX)
		left_w <= x_min_w;
		right_w <= x_max_w;
		top_w <= y_min_w;
		bottom_w <= y_max_w;
		
		//Latch edges for display overlay on next frame  (BLUE BOX)
		left_bl <= x_min_bl;
		right_bl <= x_max_bl;
		top_bl <= y_min_bl;
		bottom_bl <= y_max_bl;
		
		//Latch edges for display overlay on next frame  (NEW BOX)
		left_n <= x_min_n;
		right_n <= x_max_n;
		top_n <= y_min_n;
		bottom_n <= y_max_n;
		
		//Latch edges for display overlay on next frame  (WHITE BOX)
		left_ww <= x_min_ww;
		right_ww <= x_max_ww;
		top_ww <= y_min_ww;
		bottom_ww <= y_max_ww;
		
		
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
	distance <= (red*3 + green*6 + blue)>>2;
	
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
	else if (yellow_detect) begin
		color <= 3'b011;
		distance <= x_max_y - x_min_y;
		pos <= x_min_y[8:6];
	end
	else if (pink_detect) begin
		color <= 3'b100;
		distance <= x_max_p - x_min_p;
		pos <= x_min_p[8:6];
	end
	
	else if (green_detect) begin
		color <= 3'b101;
		distance <= x_max_g - x_min_g;
		pos <= x_min_g[8:6];
	end
	
	else if (dark_green_detect) begin
		color <= 3'b110;
		distance <= x_max_dg - x_min_dg;
		pos <= x_min_dg[8:6];
	end
	
	if (black_detect) begin
		color <= 3'b111;
		pos <= 0;
		distance <= x_max_bl - x_min_bl;
	end
	if (white_detect) begin
		color <= 3'b111;
		pos <= 1;
		distance <= x_max_ww - x_min_ww;
	end
	if (mode==0) begin
		distance <= 0;
		pos <= 0;
		color <=0;
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
reg	[23:0]	bb_col , bb_col_cyan,bb_col_w , bb_col_n;

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
		bb_col_cyan <= BB_COL_DEFAULT_CYAN;
		bb_col_dg <= BB_COL_DEFAULT_DARK_GREEN;
		bb_col_y <= BB_COL_DEFAULT_YELLOW;
		bb_col_w <= BB_COL_DEFAULT_WHITE;
		bb_col_bl <= BB_COL_DEFAULT_BLACK;
		bb_col_n <= BB_COL_DEFAULT_GREEN;
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
						
reg	[23:0]	bb_col_r, bb_col_b, bb_col_o, bb_col_p , bb_col_g , bb_col_dg , bb_col_y , bb_col_bl;

endmodule