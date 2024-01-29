	`timescale 1ns / 1ps
	module lab3_2(
				input[5:0] money,
				input CLK,
				input vm, //0:VM0, 1:VM1
				input [2:0] productID, //000:sandwich, 001:chocolate, 11x: dont care
				input sugar, //0: No sugar, 1: With Sugar
				output reg [5:0] moneyLeft,
				output reg [4:0] itemLeft,
				output reg productUnavailable,//1:show warning, 0:do not show warning
				output reg insufficientFund , //1:full, 0:not full
				output reg notExactFund , //1:full, 0:not full
				output reg invalidProduct, //1: empty, 0:not empty
				output reg sugarUnsuitable, //1: empty, 0:not empty
				output reg productReady	//1:door is open, 0:closed
		);

		// Internal State of the Module
		// (you can change this but you probably need this)
		reg [4:0] numOfSandwiches;
		reg [4:0] numOfChocolate;
		reg [4:0] numOfWaterVM0;
		reg [4:0] numOfWaterVM1;
		reg [4:0] numOfCoffee;
		reg [4:0] numOfTea;

		initial
		begin
			numOfSandwiches = 5'b01010;
			numOfChocolate = 5'b01010;
			numOfWaterVM0 = 5'b00101;
			numOfWaterVM1 = 5'b01010;
			numOfCoffee = 5'b01010;
			numOfTea = 5'b01010;
		end

		//Modify the lines below to implement your design
		always @(posedge CLK)
		begin
			moneyLeft = money;
			itemLeft = 0;
			if(vm == 0) begin // first machine
			invalidProduct = 0;
			productReady = 0;
			sugarUnsuitable = 1'bx;
			insufficientFund = 1'bx;
				 if(productID == 3'b000) begin //buys sandwich
					  productUnavailable = 0;
					  if(numOfSandwiches > 5'b00000) begin //enough product
							if(money == 6'b010100) begin // exact money
								 numOfSandwiches = numOfSandwiches - 5'b00001;
								 productReady = 1'b1;
								 itemLeft = numOfSandwiches;
								 notExactFund = 0;
								 moneyLeft = 0;
							end
							else begin // not exact money
								 notExactFund = 1'b1;
								 moneyLeft = money;
							end
					  end      
					  else begin // not enough product
							productUnavailable = 1'b1;
							moneyLeft = money;
					  end
				 end    
				 
				 else if(productID == 3'b001) begin //buys chocolate
					  productUnavailable = 0;
					  if(numOfChocolate > 5'b00000) begin //enough product
							if(money == 6'b001010) begin // exact money
								 numOfChocolate = numOfChocolate - 5'b00001;
								 productReady = 1'b1;
								 itemLeft = numOfChocolate;
								 moneyLeft = 0;
								 notExactFund = 0;
							end
							else begin // not exact money
								 notExactFund = 1'b1;
								 moneyLeft = money;
							end
					  end      
					  else begin // not enough product
							productUnavailable = 1'b1;
							moneyLeft = money;
					  end
				 end
				 
				 else if(productID == 3'b010) begin //buys water
					  productUnavailable = 0;
					  if(numOfWaterVM0 > 5'b00000) begin //enough product
							if(money == 6'b000101) begin // exact money
								 numOfWaterVM0 = numOfWaterVM0 - 5'b00001;
								 productReady = 1'b1;
								 itemLeft = numOfWaterVM0;
								 moneyLeft = 0;
								 notExactFund = 0;
							end
							else begin // not exact money
								 notExactFund = 1'b1;
								 moneyLeft = money;
							end
					  end      
					  else begin // not enough product
							productUnavailable = 1'b1;
							moneyLeft = money;
					  end
				 end
				 
				 else begin // invalid product
					  invalidProduct = 1'b1;
					  moneyLeft = money; 
					  productReady = 0;
				     itemLeft =5'bxxxxx;
					  productUnavailable = 1'bx;
				 end
			end    
			else if (vm==1) begin // second machine
			
				 if (productID == 3'b010) begin // buys water
					  productUnavailable = 0;
					  invalidProduct = 0;
					  sugarUnsuitable = 0;
					  productReady = 0;
					  if(numOfWaterVM1 > 5'b00000) begin // enough product
							if(sugar == 0) begin // does not want sugar for water
								 if(money >= 6'b000101) begin // enough money
									  numOfWaterVM1 = numOfWaterVM1 - 5'b00001;
									  productReady = 1'b1;
									  itemLeft = numOfWaterVM1;
									  moneyLeft = money - 6'b000101;
									  insufficientFund = 0;
								 end      
								 else begin // not enough money
									  insufficientFund = 1'b1;
									  moneyLeft = money;
								 end
							end
							else begin // wants sugar
								 sugarUnsuitable = 1'b1;
								 moneyLeft = money;
							end
					  end
					  else begin // not enough product
							productUnavailable = 1'b1;
							moneyLeft = money;
					  end
				 end
				 
				 else if (productID == 3'b011) begin // buys coffe
					  productUnavailable = 0;
					  invalidProduct = 0;
					  sugarUnsuitable = 0;
					  productReady = 0;
					  if (numOfCoffee > 5'b00000) begin // enough product
							if(money >= 6'b001100) begin // enough money
								 numOfCoffee = numOfCoffee - 5'b00001;
								 productReady = 1'b1;
								 itemLeft = numOfCoffee;
								 moneyLeft = money - 6'b001100;
								 insufficientFund = 0;
							end
							else begin // not enough money
								 insufficientFund = 1'b1;
								 moneyLeft = money;
							end
					  end
					  else begin // not enough product
							productUnavailable  = 1'b1;
							moneyLeft = money;
					  end
				 end
				 
				 else if (productID == 3'b100) begin // buys tea
					  productUnavailable = 0;
					  invalidProduct = 0;
					  sugarUnsuitable = 0;
					  productReady = 0;   
					  if (numOfTea > 5'b00000) begin // enough product
							if(money >= 6'b001000) begin // enough money
								 numOfTea = numOfTea - 5'b00001;
								 productReady = 1'b1;
								 itemLeft = numOfTea;
								 moneyLeft = money - 6'b001000;
								 insufficientFund = 0;
							end
							else begin // not enough money
								 insufficientFund = 1'b1;
								 moneyLeft = money;
							end
					  end
					  else begin // not enough product
							productUnavailable  = 1'b1;
							moneyLeft = money;
					  end
				 end
					else begin // invalid product
						 invalidProduct = 1'b1;
						 productReady = 0;
						 itemLeft =5'bxxxxx;
						 productUnavailable = 1'bx;
					end
			end
			else;
		end	
	endmodule
