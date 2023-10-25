
module top(
input rxd,clk,rst ,
output txd ,
inout i2c_scl,i2c_sda
);

wire scl_padoen_o , scl_pad_o , sda_pad_o , sda_padoen_o ;

assign i2c_sda = ( sda_padoen_o ==0 ) ?  sda_pad_o  : 'bz ;
assign i2c_scl = ( scl_padoen_o ==0 ) ?  scl_pad_o  : 'bz ;
wire  scl_pad_i = i2c_scl;
wire  sda_pad_i = i2c_sda;
 
  uart_burn_i2c_eeprom  #( .ClkFrequency(100*1000*1000), .Baud(115200) ) uart_burn_i2c_eeprom (
  .clk( clk  ),
  .rxd(  rxd ),
  .rst( 1'b0 ) ,
  .txd(txd  ) ,
 
  .scl_pad_i( scl_pad_i ),       
  .scl_pad_o( scl_pad_o ),    
  .scl_padoen_o( scl_padoen_o ),    

  .sda_pad_i( sda_pad_i ),       
  .sda_pad_o( sda_pad_o ),    
  .sda_padoen_o( sda_padoen_o )	

);

endmodule  







module uart_burn_i2c_eeprom (
input clk,rxd,rst ,
output txd,

	// I2C signals
	// i2c clock line
	input  scl_pad_i,       // SCL-line input
	output scl_pad_o,    // SCL-line output (always 1'b0)
	output scl_padoen_o,   // SCL-line output enable (active low)

	// i2c data line
	input  sda_pad_i,       // SDA-line input
	output sda_pad_o,    // SDA-line output (always 1'b0)
	output sda_padoen_o    // SDA-line output enable (active low) 
	
);


parameter ClkFrequency = 20*1000*1000; //20MHz
parameter Baud = 115200; 


reg [7:0] st ; 
reg [7:0] addr;

wire rx_ready ;
wire [7:0] rx_u8 ;



reg[7:0]d8 ;
always @ (posedge clk) if (rst) st<=0; else case (st)

0 : st <= 5  ;
5 : st <= 10 ; 

10 : if (rx_ready) st <= ( rx_u8=='h55 ) ? 11 : 5 ; 
11 : if (rx_ready) st <= ( rx_u8=='haa ) ? 12 : 5 ; 
12 : if (rx_ready) st <= ( rx_u8=='h5a ) ? 13 : 5 ; 
13 : if (rx_ready) begin addr <=rx_u8;st<=st+1;end 
14 : if (rx_ready) case (rx_u8)
      20: st<=20 ;   //write 8 bits 
	  30: st<=30 ;   //read 8 bits  
	  default st<= 5;endcase 
	  
	  // write 8 bits 
	  20: if (rx_ready) begin d8<=  rx_u8 ; st<=24; end  
	  24: st <= 25 ;
	  25: st <= 26 ;
	  26: st <= 27 ; 
	  27: st <= 5  ;
	  
	  //read 8 bits 
	  30: if (~tx_busy) st <= 34 ;  
	  34: st <= 35 ;
	  35: st <= 36 ;
	  36: st <= 37 ;
	  37: st <= 5  ; 
	  
	  default st <= 0 ; 
	  endcase 
 
reg [7:0]  tx_d;
reg tx_wr ;

wire[7:0]w8;
always@(posedge clk)tx_d <=  w8 ; 
always@(posedge clk)tx_wr <= 0==tx_busy && st==30 ;
reg wb_wr;  always@(posedge clk)wb_wr <= st==25;

i2c_master_top  i2c_master_top(
        .wb_clk_i( clk  ), 
        .wb_rst_i( rst ), 
        .wb_adr_i_wr( addr[2:0] ),
        .wb_adr_i_rd( addr[2:0] ), 
        .wb_dat_i(  d8 ), 
        .wb_dat_o(  w8 ),
        .wb_we_i(   wb_wr ), 
        .wb_inta_o(  ),
		
        .scl_pad_i(  scl_pad_i), 
        .scl_pad_o( scl_pad_o ), 
        .scl_padoen_o( scl_padoen_o ), 
        .sda_pad_i( sda_pad_i ), 
        .sda_pad_o( sda_pad_o ), 
        .sda_padoen_o (sda_padoen_o  ) 
);
		
async_transmitter #(  .ClkFrequency(ClkFrequency) ,.Baud(Baud))  async_transmitter (
	.clk(clk),
	.TxD_start(tx_wr),
	.TxD_data(tx_d),
	.TxD(txd),
	.TxD_busy(tx_busy)
);

async_receiver #(  .ClkFrequency(ClkFrequency) ,.Baud(Baud))  async_receiver(
	.clk(clk),
	.RxD(rxd),
	.RxD_data_ready (rx_ready),
	.RxD_data(rx_u8)  // data received, valid only (for one clock cycle) when RxD_data_ready is asserted
 );

endmodule


