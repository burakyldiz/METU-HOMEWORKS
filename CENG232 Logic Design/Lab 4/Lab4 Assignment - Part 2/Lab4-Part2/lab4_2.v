`timescale 1ns / 1ps
module SelectionOfAvatar(
	input [1:0] mode,
	input [5:0] userID,
	input [1:0] candidate, // 00:Air 01:Fire, 10:Earth, 11: Water
	input CLK,
	output reg [1:0] ballotBoxId,
	output reg [5:0] numberOfRegisteredVoters,
	output reg [5:0] numberOfVotesWinner, // number of votes of winner
	output reg [1:0] WinnerId,
	output reg AlreadyRegistered,
	output reg AlreadyVoted,
	output reg NotRegistered,
	output reg VotingHasNotStarted,
	output reg RegistrationHasEnded
	);
	
	integer cycle_count;
		reg registered00[15:0];
		reg registered01[15:0];
		reg registered10[15:0];
		reg registered11[15:0];
		reg voted00[15:0];
		reg voted01[15:0];
		reg voted10[15:0];
		reg voted11[15:0];
		integer check_no;
		integer box_no;
		integer air;
		integer fire;
		integer earth;
		integer water;
		integer i;
		integer register_count;
	
	initial begin
		// ...
		cycle_count=0;
		for (i=0; i<16; i=i+1) begin
			registered00[i] = 0;
			registered01[i] = 0;
			registered10[i] = 0;
			registered11[i] = 0;
			voted00[i] = 0;
			voted01[i] = 0;
			voted10[i] = 0;
			voted11[i] = 0;
		end
		box_no=0;
		air=0;
		fire=0;
		earth=0;
		water=0;
		numberOfRegisteredVoters = 6'b000000;
		numberOfVotesWinner = 6'b000000;
		WinnerId = 2'b00;
	end

	always @(posedge CLK)
	begin
		// ...
		cycle_count = cycle_count + 1;
		check_no = 0;
		AlreadyRegistered =0;
		AlreadyVoted=0;
		NotRegistered=0;
		VotingHasNotStarted =1'b0;
		RegistrationHasEnded = 0;
		ballotBoxId[0] = userID[4];
		ballotBoxId[1] = userID[5];

		if (cycle_count <= 100) begin
			if(mode == 2'b01) begin
				VotingHasNotStarted = 1'b1;
			end
			else if (mode == 1'b0) begin
				
				check_no = userID[0] + 2*userID[1] + 4*userID[2] + 8*userID[3];
				
				if(ballotBoxId == 2'b00) begin
					if(registered00[check_no] == 1) begin
						AlreadyRegistered = 1'b1;
					end
					else if(registered00[check_no] == 0) begin
						registered00[check_no] = 1;
						numberOfRegisteredVoters = numberOfRegisteredVoters + 6'b000001;

					end
				end
				
				if(ballotBoxId == 2'b01) begin
					if(registered01[check_no] == 1) begin
						AlreadyRegistered = 1'b1;
					end
					else if(registered01[check_no] == 0) begin
						registered01[check_no] = 1;
						numberOfRegisteredVoters = numberOfRegisteredVoters + 6'b000001;
					end
				end
				
				if(ballotBoxId == 2'b10) begin
					if(registered10[check_no] == 1) begin
						AlreadyRegistered = 1'b1;
					end
					else if(registered10[check_no] == 0) begin
						registered10[check_no] = 1;
						numberOfRegisteredVoters = numberOfRegisteredVoters + 6'b000001;
					end
				end
				
				if(ballotBoxId == 2'b11) begin
					if(registered11[check_no] == 1) begin
						AlreadyRegistered = 1'b1;
					end
					else if(registered11[check_no] == 0) begin
						registered11[check_no] = 1;
						numberOfRegisteredVoters = numberOfRegisteredVoters + 6'b000001;
					end
				end
			end
			
		end
		
		else if(cycle_count > 100 && cycle_count <= 200 && mode == 1) begin
			VotingHasNotStarted = 1'b0;
			check_no = userID[0] + 2*userID[1] + 4*userID[2] + 8*userID[3];
			box_no = 2*userID[5] + userID[4];
			if(box_no == 0)begin
				if(registered00[check_no] == 0) begin
					NotRegistered = 1'b1;
				end
				else begin
					if(voted00[check_no] == 1) begin
						AlreadyVoted = 1'b1;
					end
					else begin
						voted00[check_no] = 1;
						if(candidate[1] == 0 && candidate[0] == 0) begin
							air = air +1;
						end
						else if(candidate[1] == 0 && candidate[0] == 1) begin
							fire = fire + 1;
						end
						else if(candidate[1] == 1 && candidate[0] == 0) begin
							water = water +1;
						end
						else if (candidate[1] == 1 && candidate[0] == 1) begin
							
							earth = earth +1;
						end
					end
				end
			end
			
			else if(box_no == 1)begin
				if(registered01[check_no] == 0) begin
					NotRegistered = 1'b1;
				end
				else begin
					if(voted01[check_no] == 1) begin
						AlreadyVoted = 1'b1;
					end
					else begin
						voted01[check_no] = 1;
						if(candidate[0] == 0 && candidate[1] == 0) begin
							air = air +1;
						end
						else if(candidate[1] == 0 && candidate[0] == 1) begin
							fire = fire + 1;
						end
						else if(candidate[1] == 1 && candidate[0] == 0) begin
							
							water = water +1;
						end
						else if (candidate[0] == 1 && candidate[1] == 1) begin
							earth = earth +1;
						end
					end
				end
			end
			
			else if(box_no == 2)begin
				if(registered10[check_no] == 0) begin
					NotRegistered = 1'b1;
				end
				else begin
					if(voted10[check_no] == 1) begin
						AlreadyVoted = 1'b1;
					end
					else begin
						voted10[check_no] = 1;
						if(candidate[0] == 0 && candidate[1] == 0) begin
							air = air +1;
						end
						else if(candidate[1] == 0 && candidate[0] == 1) begin
							fire = fire + 1;
						end
						else if(candidate[1] == 1 && candidate[0] == 0) begin
							water = water +1;
						end
						else if (candidate[0] == 1 && candidate[1] == 1) begin
							
							earth = earth +1;

						end
					end
				end
			end
			
			else if(box_no == 3)begin
				if(registered11[check_no] == 0) begin
					NotRegistered = 1'b1;
				end
				else begin
					if(voted11[check_no] == 1) begin
						AlreadyVoted = 1'b1;
					end
					else begin
						voted11[check_no] = 1;
						if(candidate[0] == 0 && candidate[1] == 0) begin
							air = air +1;
						end
						else if(candidate[1] == 0 && candidate[0] == 1) begin
							fire = fire + 1;
						end
						else if(candidate[1] == 1 && candidate[0] == 0) begin
							water = water +1;
						end
						else if (candidate[1] == 1 && candidate[0] == 1) begin
							earth = earth +1;
						end
					end
				end
			end
		end
		else if(cycle_count > 100 && cycle_count <= 200 && mode == 0) begin
			RegistrationHasEnded = 1'b1;
		end
		
		else if ( cycle_count > 200) begin
			ballotBoxId = 2'bxx;
			AlreadyRegistered = 1'bx;
			AlreadyVoted = 1'bx;
			NotRegistered = 1'bx;
			VotingHasNotStarted = 1'bx;
			RegistrationHasEnded = 1'bx;
			if(air > fire && air > earth && air > water) begin
				numberOfVotesWinner = air;
				WinnerId = 2'b00;
			end
			
			else if ( fire > air && fire > earth && fire > water) begin
				numberOfVotesWinner = fire;
				WinnerId = 2'b01;
			end
			
			else if (water > air && water > fire && water > earth) begin
				numberOfVotesWinner = water;
				WinnerId = 2'b10;
			end
			
			else if (earth > air && earth > fire && earth > air) begin
				numberOfVotesWinner = earth;
				WinnerId = 2'b11;
			end
		end
	end

endmodule
