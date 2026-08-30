// 0x52 for echo
// 0x0? for read
// 0x1? for rst Ch

module spi_decoder (
    input        byte_done,
    input  [7:0] rx_shift,

    input  [7:0] CH1,
    input  [7:0] CH2,
    input  [7:0] CH3,
    input  [7:0] CH4,
    input  [7:0] CH5,
    input  [7:0] CH6,
    input  [7:0] CH7,
    input  [7:0] CH8,
    input  [7:0] CH9,
    input  [7:0] CH10,
    input  [7:0] CH11,
    input  [7:0] CH12,
    input  [7:0] CH13,
    input  [7:0] CH14,
    input  [7:0] CH15,

    output     [7:0] tx,
    output     [14:0] CH_en,
    output     [14:0] CH_rst
);

assign CH_en =
    byte_done &&
    (rx_shift >= 8'h01) &&
    (rx_shift <= 8'h0F)
        ? (15'b1 << (rx_shift - 8'h01))
        : 15'b0;

assign CH_rst =
    byte_done &&
    (rx_shift >= 8'h11) &&
    (rx_shift <= 8'h1F)
        ? (15'b1 << (rx_shift - 8'h11))
        : 15'b0;
        
assign tx =
    CH_en[0]  ? CH1  :
    CH_en[1]  ? CH2  :
    CH_en[2]  ? CH3  :
    CH_en[3]  ? CH4  :
    CH_en[4]  ? CH5  :
    CH_en[5]  ? CH6  :
    CH_en[6]  ? CH7  :
    CH_en[7]  ? CH8  :
    CH_en[8]  ? CH9  :
    CH_en[9]  ? CH10 :
    CH_en[10] ? CH11 :
    CH_en[11] ? CH12 :
    CH_en[12] ? CH13 :
    CH_en[13] ? CH14 :
    CH_en[14] ? CH15 :
                rx_shift;   
endmodule

