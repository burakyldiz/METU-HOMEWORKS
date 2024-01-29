`timescale 1ns / 1ps

module ROM (input [2:0] addr,output reg [7:0] dataOut);
	always @(*) begin
        case(addr)
    	    3'b000: dataOut = 8'b00000000;
    	    3'b001: dataOut = 8'b01010101;
    	    3'b010: dataOut = 8'b10101010;
    	    3'b011: dataOut = 8'b00110011;
    	    3'b100: dataOut = 8'b11001100;
    	    3'b101: dataOut = 8'b00001111;
    	    3'b110: dataOut = 8'b11110000;
    	    3'b111: dataOut = 8'b11111111;
        endcase
    end
endmodule

module Difference_RAM (input mode,input [2:0] addr,input [7:0] dataIn,input [7:0] mask,input CLK,output reg [7:0] dataOut);
    
    reg[7:0] ram[7:0];
    integer i, res;
    
    initial begin
        dataOut = 8'd0;
        for(i=0; i < 8; i = i+1) begin
            ram[i] = 8'd0;
        end
        i=0;
        res = 0;
    end
    
    always @(mode) begin
        if (mode == 1) begin // read
            dataOut = ram[addr];
        end
    end
    
    always @(posedge CLK) begin
        if (mode == 0) begin // write
            res = dataIn - mask;
				if(res <= 0) begin
					res = -res;
				end
            ram[addr] = res;
        end
	 end
endmodule


module EncodedMemory (input mode,input [2:0] index,input [7:0] number,input CLK,output [7:0] result);

	wire [7:0] mask;

	ROM R(index, mask);
	Difference_RAM DR(mode, index, number, mask, CLK, result);

endmodule


