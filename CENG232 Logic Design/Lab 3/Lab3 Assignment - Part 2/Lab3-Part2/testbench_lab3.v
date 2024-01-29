`timescale 1ns / 1ps

module testbench;
	// Inputs
	reg[5:0] Money;
	reg clk;
	reg VM;
	reg[2:0] ProductID;
	reg Sugar;
	// Outputs
	wire[5:0] MoneyLeft;
	wire[4:0] ItemLeft;
	wire ProductUnavailable;
	wire InsufficientFund;
	wire InvalidProduct;
	wire SugarUnsuitable;
	wire ProductReady;

	// Instantiate UUT
	lab3_2 uut (.money(Money),
				.CLK(clk),
				.vm(VM),
				.productID(ProductID),
				.sugar(Sugar),
				.moneyLeft(MoneyLeft),
				.itemLeft(ItemLeft),
				.productUnavailable(ProductUnavailable),
                .insufficientFund(InsufficientFund),
                .notExactFund(I2),
                .invalidProduct(InvalidProduct),
                .sugarUnsuitable(SugarUnsuitable),
                .productReady(ProductReady));

    // Clock Related
    // At 5, 15, 25, .. clk will change 0->1
	// At 10, 20, 30, .. clk will change 1->0
    initial clk = 0;
    always #5 clk = ~clk;

	initial begin

        // You can implement your test here...

		$finish;
	end

endmodule

