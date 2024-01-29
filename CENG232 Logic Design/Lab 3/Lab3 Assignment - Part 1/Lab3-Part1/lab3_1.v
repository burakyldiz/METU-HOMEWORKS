`timescale 1ns / 1ps

module bh(input B, input H, input clk, output reg Q);

    initial begin
        Q = 1;
    end

	// You can implement your code here
    // ...
     always@(posedge clk)
        case({B,H})
            2'b00: Q <= ~Q;
            2'b01: Q <= 1;
            2'b10: Q <= 0;
            2'b11: Q <= Q;
        endcase

endmodule

module ic1337(// Inputs
              input A0,
              input A1,
              input A2,
              input clk,
              // Outputs
              output Q0,
              output Q1,
              output Z);

	// You can implement your code here
    // ...
        /* First Flip Flop: Q0 */
        wire X1,Y1 ;
     
        assign X1 = (A2) | ((A0) ^ (~A1));
        assign Y1 = (A0) & (~A2);
     
        bh bh1(X1, Y1, clk, Q0);
         
        /* Second Flip Flop: Q1 */
        wire X2, Y2;
     
        assign X2 = (A0) & (~A2);
        assign Y2 = (~(A1 | (~A0))) & (A2);
     
        bh bh2(X2, Y2, clk, Q1);
     
        /* Z */
        assign Z = Q0 ~^ Q1;

endmodule