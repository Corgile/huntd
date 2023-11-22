//
// Created by brian on 11/22/23.
//

#include <hound/entity/capture_option.hpp>
#include <hound/common/hd_macro.hpp>


hd::entity::capture_option::capture_option() {
	/// filter
	this->include_radiotap = false;
	this->include_wlan = false;
	this->include_eth = false;
	this->include_ipv4 = false;
	this->include_ipv6 = false;
	this->include_tcp = false;
	this->include_udp = false;
	this->include_icmp = false;
	this->include_vlan = false;
	this->timestamp = false;
	this->verbose = false;
	this->unsign = false;
	this->caplen = false;
	/// mode
	this->send_kafka = false;
	this->write_file = false;
	this->offline_mode = false;
	this->live_mode = true;
	/// config
	this->payload_len = 0;
	this->num_packets = -1;
	this->fill_bit = 0;
	this->min_packets = 5;
	this->max_packets = 100;
	this->interval = 20;
	this->duration = 0;
	this->stride = 8;
	this->output_index = 4;
	this->kafka_config = {};
	this->out_path = {};
	this->filter = {};
	this->device = {};

	this->index_map = {
			{0, "src_ip"},
			{1, "dst_ip"},
			{2, "src_prt"},
			{3, "dst_prt"},
			{4, "flow_id"}
	};
}

void hd::entity::capture_option::print() {

	INFO(CYAN("当前运行参数: "));
	if (include_radiotap)
		INFO(CYAN("radiotap = "), "true");
	if (include_wlan)
		INFO(CYAN("wlan     = "), "true");
	if (include_eth)
		INFO(CYAN("eth      = "), "true");
	if (include_ipv4)
		INFO(CYAN("ipv4     = "), "true");
	if (include_ipv6)
		INFO(CYAN("ipv6     = "), "true");
	if (include_tcp)
		INFO(CYAN("tcp      = "), "true");
	if (include_udp)
		INFO(CYAN("udp      = "), "true");
	if (include_icmp)
		INFO(CYAN("icmp     = "), "true");
	if (include_vlan)
		INFO(CYAN("vlan     = "), "true");
	if (caplen)
		INFO(CYAN("caplen   = "), "true");
	if (timestamp)
		INFO(CYAN("time     = "), "true");
	if (payload_len > 0)
		INFO(CYAN("payload  = "), payload_len, " Bytes");
	if (num_packets <= 0) {
		INFO(CYAN("Packets  = "), "INF");
	} else {
		INFO(CYAN("Packets  = "), num_packets);
	}
	INFO(CYAN("stride   = "), stride);
	INFO(CYAN("fill_bit = "), fill_bit);
	if (unsign) INFO(CYAN("unsigned = "), "true");
	else
		INFO(CYAN("unsigned = "), "false");
	if (live_mode) {
		INFO(CYAN("min_pkt  = "), min_packets);
		INFO(CYAN("max_pkt  = "), max_packets);

	}
	INFO(CYAN("workers = "), workers);
	if (this->output_index not_eq -1) {
		INFO(CYAN("index    = "), this->index_map[output_index]);
	}
	if (duration > 0) {
		INFO(CYAN("duration = "), duration, "秒");
	}
	if (send_kafka and not kafka_config.empty()) {
		INFO(CYAN("config   = "), kafka_config);
	}
	if (this->write_file and not out_path.empty()) {
		INFO(CYAN("output   = "), out_path);
	}
	if (this->offline_mode and not pcap_file.empty()) {
		INFO(CYAN("pcap_file = "), pcap_file);
	}
	if (not device.empty()) {
		INFO(CYAN("device    = "), device);
	}
}