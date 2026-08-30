module adc_timing_controller
(
    input wire clk,

    // Asynchronous ADC BUSY input
    input wire busy,

    output reg sck,
    output reg shift_enable,
    output reg adc_data_valid
);


////////////////////////////////////////////////////
// BUSY synchronization
////////////////////////////////////////////////////

reg busy_sync1;
reg busy_sync2;
reg busy_prev;


wire busy_falling;


// Falling edge detection
assign busy_falling = busy_prev & ~busy_sync2;



////////////////////////////////////////////////////
// FSM states
////////////////////////////////////////////////////

localparam IDLE  = 2'd0;
localparam SHIFT = 2'd1;
localparam DONE  = 2'd2;


reg [1:0] state = IDLE;



////////////////////////////////////////////////////
// SCK pulse counter
////////////////////////////////////////////////////

reg [4:0] pulse_cnt;



////////////////////////////////////////////////////
// Main controller
////////////////////////////////////////////////////

always @(posedge clk)
begin


    ////////////////////////////////////////////////////
    // Synchronize BUSY
    ////////////////////////////////////////////////////

    busy_sync1 <= busy;
    busy_sync2 <= busy_sync1;
    busy_prev  <= busy_sync2;



    ////////////////////////////////////////////////////
    // Default values
    ////////////////////////////////////////////////////

    shift_enable <= 0;



    ////////////////////////////////////////////////////
    // FSM
    ////////////////////////////////////////////////////

    case(state)



    ////////////////////////////////////////////////////
    // Wait for BUSY falling edge
    ////////////////////////////////////////////////////

    IDLE:
    begin

        sck <= 0;

        pulse_cnt <= 0;

        adc_data_valid <= 0;



        if(busy_falling)
        begin
        
            shift_enable <= 1;   // capture D15
            state <= SHIFT;

        end


    end



    ////////////////////////////////////////////////////
    // Generate 15 SCK pulses
    ////////////////////////////////////////////////////

    SHIFT:
    begin


        if(sck == 0)
        begin

            ////////////////////////////////////////////////////
            // SCK rising edge
            ////////////////////////////////////////////////////

            sck <= 1;


            // tell shift register to sample DOUT
            shift_enable <= 1;



            if(pulse_cnt == 5'd14)
            begin

                state <= DONE;

            end

            else
            begin

                pulse_cnt <= pulse_cnt + 1;

            end


        end


        else
        begin

            ////////////////////////////////////////////////////
            // SCK falling edge
            ////////////////////////////////////////////////////

            sck <= 0;

        end


    end

    DONE:
    begin
    
        sck <= 0;
    
        adc_data_valid <= 1;
    
        state <= IDLE;
    
    end
    default:
    begin

        state <= IDLE;

        sck <= 0;

    end


    endcase


end


endmodule