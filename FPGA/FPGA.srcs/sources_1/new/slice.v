`timescale 1ns / 1ps

module slice (
    input  [14:0] Din,
    output       D0,
    output       D1,
    output       D2,
    output       D3,
    output       D4,
    output       D5,
    output       D6,
    output       D7,
    output       D8,
    output       D9,
    output       D10,
    output       D11,
    output       D12,
    output       D13,
    output       D14
);

    assign D0 = Din[0];
    assign D1 = Din[1];
    assign D2 = Din[2];
    assign D3 = Din[3];
    assign D4 = Din[4];
    assign D5 = Din[5];
    assign D6 = Din[6];
    assign D7 = Din[7];
    assign D8 = Din[8];
    assign D9 = Din[9];
    assign D10 = Din[10];
    assign D11 = Din[11];
    assign D12 = Din[12];
    assign D13 = Din[13];
    assign D14 = Din[14];
    
endmodule
