module fifo_controller
(
    input wire clk,

    input wire adc_data_valid,

//    input wire fifo_full,
//    input wire fifo_wr_rst_busy,

    output reg fifo_wr_en
);


always @(posedge clk)
begin

    fifo_wr_en <= 1'b0;


//    if(adc_data_valid &&
//       !fifo_full &&
//       !fifo_wr_rst_busy)
   if(adc_data_valid)
    begin

        fifo_wr_en <= 1'b1;

    end

end


endmodule