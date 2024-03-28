`timescale 1 ns / 1 ps
//`default_nettype none


module pit_sv #(
    // Width of S_AXI address bus
    parameter integer C_S_AXI_ADDR_WIDTH = 4
)
(
    input logic                             s_axi_aclk,
    input logic                             s_axi_aresetn,

    // AW
    input logic                             s_axi_awvalid,
    output logic                            s_axi_awready,
    input logic [C_S_AXI_ADDR_WIDTH-1:0]    s_axi_awaddr,
    input logic [2:0]                       s_axi_awprot,

    // W
    input logic                             s_axi_wvalid,
    output logic                            s_axi_wready,
    input logic [31:0]                      s_axi_wdata,
    input logic [3:0]                       s_axi_wstrb,

    // B
    output logic                            s_axi_bvalid,
    input logic                             s_axi_bready,
    output logic [1:0]                      s_axi_bresp,

    // AR
    input logic                             s_axi_arvalid,
    output logic                            s_axi_arready,
    input logic [C_S_AXI_ADDR_WIDTH-1:0]    s_axi_araddr,
    input logic [2:0]                       s_axi_arprot,

    // R
    output logic [31:0]                     s_axi_rdata,
    output logic [1:0]                      s_axi_rresp,
    output logic                            s_axi_rvalid,
    input logic                             s_axi_rready,

    output logic                            irq
);

// Add your module here //

// states declaration
typedef enum logic [2:0] { WRITE_IDLE, WRITE_WAIT_HANDSHAKE, WRITE_WAIT_RESPONSE } write_states;
typedef enum logic [2:0] { READ_IDLE, READ_WAIT_HANDSHAKE, READ_WAIT_RESPONSE } read_states;
write_states cs_write;
read_states cs_read;
    
// local logics
logic reset;
logic [31:0] timer_counter;
logic [31:0] control_register;
logic [31:0] delay_value_register;
logic [C_S_AXI_ADDR_WIDTH-1:0] write_address;
logic [C_S_AXI_ADDR_WIDTH-1:0] read_address;

// constants
localparam CONTROL_REG_OFFSET = 32'h00;
localparam DELAY_REG_OFFSET = 32'h04;

// output ports assignments
assign s_axi_awready = (cs_write == WRITE_WAIT_HANDSHAKE);
assign s_axi_wready = (cs_write == WRITE_WAIT_HANDSHAKE);
assign s_axi_bvalid = (cs_write == WRITE_WAIT_RESPONSE);
assign s_axi_arready = (cs_read == READ_WAIT_HANDSHAKE);
assign s_axi_rvalid = (cs_read == READ_WAIT_RESPONSE);

// not used, default to low 
assign s_axi_bresp = 0;
assign s_axi_rresp = 0;

// assign reset signal
assign reset = !s_axi_aresetn;


// write 
always_ff @(posedge s_axi_aclk) begin
    if (reset) begin 
        cs_write <= WRITE_IDLE;
        delay_value_register <= 0;
        control_register <= 0;
    end
    else begin
        if (cs_write == WRITE_IDLE) begin
            if (s_axi_awvalid && s_axi_wvalid) begin
                cs_write <= WRITE_WAIT_HANDSHAKE;
                write_address = s_axi_awaddr;
            end
            else cs_write <= cs_write;
        end
        else if (cs_write == WRITE_WAIT_HANDSHAKE) begin
            cs_write <= WRITE_WAIT_RESPONSE;
            if (write_address == CONTROL_REG_OFFSET) control_register <= s_axi_wdata;
            else delay_value_register <= s_axi_wdata;
        end
        else begin 
            if (s_axi_bvalid && s_axi_bready) cs_write <= WRITE_IDLE;
            else cs_write <= cs_write;
        end
    end
end


// read
always_ff @(posedge s_axi_aclk) begin
    if (reset) cs_read <= READ_IDLE;
    else begin
        if (cs_read == READ_IDLE) begin 
            if (s_axi_arvalid) begin 
                cs_read <= READ_WAIT_HANDSHAKE;
                read_address <= s_axi_araddr;
            end
            else cs_read <= cs_read;
        end
        else if (cs_read == READ_WAIT_HANDSHAKE) begin
            cs_read <= READ_WAIT_RESPONSE;
            if (read_address == CONTROL_REG_OFFSET) s_axi_rdata <= control_register;
            else s_axi_rdata <= delay_value_register;
        end
        else begin
            if (s_axi_rvalid && s_axi_rready) cs_read <= READ_IDLE;
            else cs_read <= cs_read;
        end
    end
end


// counter
always_ff @(posedge s_axi_aclk) begin 
    if (reset) timer_counter <= 0;
    else begin 
        if (s_axi_wready && write_address == DELAY_REG_OFFSET) timer_counter <= s_axi_wdata - 1;
        
        // if bit 0 is high -- enables the counter to decrement
        else if (control_register[0]) begin

            // when the timer-counter reaches 0, it is reloaded based on the delay-value register
            if (timer_counter == 0) begin
                timer_counter <= delay_value_register -1;
                
                // if bit 1 is high -- enables interrupts
                if (control_register[1]) irq <= 1;
            end
            else begin
                timer_counter <= timer_counter -1;
                irq <= 0;
            end
        end
        else timer_counter <= timer_counter;
    end
end

































//// write states
//always_comb begin
//	case (cs_write)
//		WRITE_IDLE: 
//			if (s_axi_awvalid && s_axi_wvalid)
//                ns_write = WRITE_HANDSHAKE;
//			else ns_write = cs_write;
//		WRITE_HANDSHAKE:
//			ns_write = RESPONSE;
//		RESPONSE:
//            if (s_axi_bvalid && s_axi_bready) ns_write = WRITE_IDLE;
//			else ns_write = cs_write;
//		default: 
//			ns_write = WRITE_IDLE;
//	endcase
//end

//// read states
//always_comb begin
//	case (cs_read)
//		READ_IDLE:
//			if (s_axi_arvalid) ns_read = READ_HANDSHAKE;
//			else ns_read = cs_read;
//		READ_HANDSHAKE: 
//			ns_read = TRANSFER;
//		TRANSFER:
//			if (s_axi_rvalid && s_axi_rready) ns_read = READ_IDLE;
//			else ns_read = cs_read;
//		default: 
//			ns_read = READ_IDLE;
//	endcase
//end

//// state update
//always_ff @(posedge s_axi_aclk) begin
//    if (s_axi_aresetn) begin 
//		reset <= !s_axi_aresetn;
//        cs_write <= WRITE_IDLE;
//		cs_read <= READ_IDLE;
//	end
//	else begin
//		reset <= 0;
//		cs_write <= ns_write;
//		cs_read <= ns_read;
//	end
//end


//// write
//always_comb begin

//    // defaults
//    delay_register = 0;
//    control_register = 0;
//    write_address = 0;
    
//	if (reset) begin
//        delay_register = 0;
//        control_register = 0;
//    end
//    else begin
//        case (cs_write)
//            WRITE_IDLE: begin
//                if (s_axi_awvalid && s_axi_wvalid) write_address = s_axi_awaddr;
//                else write_address = 0;
//            end
            
//            WRITE_HANDSHAKE: begin
//                case (write_address)
//                    CONTROL_REG_OFFSET: control_register = s_axi_wdata;
//                    DELAY_REG_OFFSET: delay_register = s_axi_wdata;
//                endcase
//            end        
//        endcase
//    end
//end


//// read
//always_comb begin

//    // defaults
//    read_address = 0;
//    s_axi_rdata = 0;
    
//	case (cs_read)
//		READ_IDLE:
//			if (s_axi_arvalid) read_address = s_axi_araddr;
		
//		READ_HANDSHAKE: begin
//			case (read_address)
//				CONTROL_REG_OFFSET: s_axi_rdata = control_register;
//				DELAY_REG_OFFSET: s_axi_rdata = delay_register;
//			endcase
//		end
//	endcase
//end


//// counter
//always_ff @(posedge s_axi_aclk) begin 

//    if (s_axi_aresetn) timer_counter <= 0;

//    else begin 
//        if (s_axi_wready && write_address == DELAY_REG_OFFSET) timer_counter <= s_axi_wdata - 1;
        
//        else if (control_register[0]) begin

//            if (timer_counter == 0) begin
//                timer_counter <= delay_register -1;
//                if (control_register[1]) irq <= 1;
//            end
//            else begin
//                timer_counter <= timer_counter -1;
//                irq <= 0;
//            end
//        end
//    end
//end

endmodule


