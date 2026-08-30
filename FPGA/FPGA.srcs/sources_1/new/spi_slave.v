//SCLK idles low
//data sampled on posedge
//data changed on negedge

module spi_slave (
    input            sclk,
    input            mosi,
    input            cs,
    output  reg[7:0] rx_shift,
    output           miso,
    output           byte_done,
    input      [7:0] tx
);
reg [2:0] bit_cnt;
reg [7:0] tx_shift;

assign byte_done = !cs && (bit_cnt == 3'd7);
assign miso = cs ? 1'bz : tx_shift[7];

always @(posedge sclk or posedge cs) begin
    if (cs) begin
        bit_cnt  <= 3'd7;
        rx_shift <= 8'd0;
    end else begin
        rx_shift <= {rx_shift[6:0], mosi};
        bit_cnt  <= bit_cnt + 3'd1;
    end
end

always @(negedge sclk) begin
    if (byte_done) begin
        tx_shift <= tx;
    end else begin
        tx_shift <= {tx_shift[6:0], 1'b0};
    end
end



endmodule