`timescale 1ns / 1ps

import axi_vip_pkg::*;
import design_1_axi_vip_0_0_pkg::*;

module pit_tb();
     
  bit clock;
  bit reset_n;
  bit irq;
  
  design_1_axi_vip_0_0_mst_t master_agent;
   
  xil_axi_ulong addrCtrl = 32'h44A0_0000;
  xil_axi_ulong addrPeriod = 32'h44A0_0004;
  xil_axi_prot_t prot = 0;
  xil_axi_resp_t resp;
  bit[31:0] data;
  
  // instantiate block diagram design
  design_1 design_1_i (
	.aclk_0(clock),
    .aresetn_0(reset_n),
    .irq_0(irq));
  
  always #5ns clock <= ~clock;

  initial begin
    master_agent = new("master vip agent",design_1_i.axi_vip_0.inst.IF);
    
    //Start the agent
    master_agent.start_master();
    
    #200ns
    reset_n = 1'b0;
	
	#200ns
    reset_n = 1'b1;
    
    #50ns
    data = 32'd2;
    master_agent.AXI4LITE_WRITE_BURST(addrPeriod, prot, data, resp);
	$display("Write '2' to delay-value register: %d", data);
    
    #50ns
    data = 32'h00000003;
    master_agent.AXI4LITE_WRITE_BURST(addrCtrl, prot, data, resp);
	$display("Write to control register: %d", data);
	
	#50ns
    master_agent.AXI4LITE_READ_BURST(addrPeriod, prot, data, resp);
	$display("Read from delay-value register: %d", data);
    
    #50ns
    master_agent.AXI4LITE_READ_BURST(addrCtrl, prot, data, resp);
	$display("Read from control register: %d", data);

	#50ns
    data = 32'h00000000;
    master_agent.AXI4LITE_WRITE_BURST(addrCtrl, prot, data, resp);
	$display("Disabled PIT and Interrupt.");
	
	#50ns
    data = 32'h00000001;
    master_agent.AXI4LITE_WRITE_BURST(addrCtrl, prot, data, resp);
	$display("Enabled PIT. Disabled Interrupt.");
	
	#50ns
    data = 32'h00000002;
    master_agent.AXI4LITE_WRITE_BURST(addrCtrl, prot, data, resp);
	$display("Enabled Interrupt. Disabled PIT.");
	
	#50ns
    data = 32'd4;
    master_agent.AXI4LITE_WRITE_BURST(addrPeriod, prot, data, resp);
	$display("Write '4' to delay-value register: %d", data);
	
	#50ns
    data = 32'h00000003;
    master_agent.AXI4LITE_WRITE_BURST(addrCtrl, prot, data, resp);
	$display("Enabled PIT. Enabled Interrupt.");
	
	#50ns
    master_agent.AXI4LITE_READ_BURST(addrPeriod, prot, data, resp);
	$display("Read from delay-value register: %d", data);
    
    #50ns
    master_agent.AXI4LITE_READ_BURST(addrCtrl, prot, data, resp);
	$display("Read from control register: %d", data);
	
	#500ns // run long enough to general interrupts
	
    $finish;

  end

endmodule