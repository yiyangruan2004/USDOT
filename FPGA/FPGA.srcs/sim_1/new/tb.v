`timescale 1ns/1ps

module tb;

    wire sclk;
    reg clk;
    reg mosi;
    wire miso;
    reg cs;

    // DUT
    design_1_wrapper dut (
        .cs(cs),
        .mosi(mosi),
        .miso(miso),
        .sclk(sclk)
    );

    // -------------------------
    // clock generation
    // -------------------------
    initial clk = 0;
    always #5 clk = ~clk;
    assign sclk = (cs)?0:clk;


    // -------------------------
    // stimulus
    // -------------------------
    initial begin
        cs   = 1;
        mosi = 0;
        #100;
        initial_byte(8'h52);  
        end_byte(8'h00);
        cs = 1;
        mosi = 0;
        #50;
        initial_byte(8'h11);
        send_byte(8'h01);
        send_byte(8'h01);
        send_byte(8'h01);
        send_byte(8'h01);
        send_byte(8'h01);
        end_byte(8'h00);
        #50;
        cs = 1;
        mosi = 0;
        #300;

        $stop;
    end


    
    // -------------------------
    // SPI BYTE TASK
    // -------------------------
    task initial_byte(input [7:0] data);
        integer i;
        begin
            cs = 0;
            mosi = data[7];
            for (i = 6; i >= 0; i = i - 1) begin
                @(negedge sclk);
                mosi = data[i];
            end
        end
    endtask
    
    task send_byte(input [7:0] data);
        integer i;
        begin
            for (i = 7; i >= 0; i = i - 1) begin
                @(negedge sclk);
                mosi = data[i];
            end
        end
    endtask
    
    task end_byte(input [7:0] data);
        integer i;
        begin
            for (i = 7; i >= 0; i = i - 1) begin
                @(negedge sclk);
                mosi = data[i];
            end
            @(negedge sclk);
        end
    endtask
    
    

endmodule