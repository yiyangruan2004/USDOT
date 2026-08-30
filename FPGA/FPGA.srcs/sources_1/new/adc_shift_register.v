module adc_shift_register
(
    input wire clk,

    input wire shift_enable,
    input wire dout,

    input wire data_valid,

    output reg signed [15:0] adc_sample
);


reg signed [15:0] shift_reg;



always @(posedge clk)
begin


if(shift_enable)
begin

    shift_reg <= {shift_reg[14:0],dout};

end



if(data_valid)
begin

    adc_sample <= shift_reg;

end


end


endmodule