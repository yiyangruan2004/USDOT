module cnv_generator_1
#(
    parameter CLK_FREQ = 12_000_000,
    parameter SAMPLE_FREQ = 180_000
)
(
    input wire clk,

    output reg cnv

);


localparam CNV_PERIOD =
(CLK_FREQ / SAMPLE_FREQ) +
((CLK_FREQ % SAMPLE_FREQ) >= (SAMPLE_FREQ/2));


localparam CNV_HIGH = 1;


reg [15:0] counter = 16'd0;


always @(posedge clk)
begin




    if(counter == CNV_PERIOD-1)
        counter <= 0;
    else
        counter <= counter + 1;


    if(counter < CNV_HIGH)
        cnv <= 1;
    else
        cnv <= 0;





end


endmodule