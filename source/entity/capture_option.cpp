//
// Created by brian on 11/22/23.
//

#include <hound/common/macro.hpp>
#include <hound/entity/capture_option.hpp>


hd::entity::capture_option::capture_option() {
	/// filter
	this->include_radiotap = false;
	this->include_wlan = false;
	this->include_eth = false;
	this->include_ip4 = false;
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
	hd_info(CYAN("当前运行参数: "));
	if (include_radiotap)
		hd_info(CYAN("radiotap = "), "true");
	if (include_wlan)
		hd_info(CYAN("wlan     = "), "true");
	if (include_eth)
		hd_info(CYAN("eth      = "), "true");
	if (include_ip4)
		hd_info(CYAN("ipv4     = "), "true");
	if (include_ipv6)
		hd_info(CYAN("ipv6     = "), "true");
	if (include_tcp)
		hd_info(CYAN("tcp      = "), "true");
	if (include_udp)
		hd_info(CYAN("udp      = "), "true");
	if (include_icmp)
		hd_info(CYAN("icmp     = "), "true");
	if (include_vlan)
		hd_info(CYAN("vlan     = "), "true");
	if (caplen)
		hd_info(CYAN("caplen   = "), "true");
	if (timestamp)
		hd_info(CYAN("time     = "), "true");
	if (payload_len > 0)
		hd_info(CYAN("payload  = "), payload_len, " Bytes");
	if (num_packets <= 0) {
		hd_info(CYAN("Packets  = "), "INF");
	} else {
		hd_info(CYAN("Packets  = "), num_packets);
	}
	hd_info(CYAN("stride   = "), stride);
	hd_info(CYAN("fill_bit = "), fill_bit);
	if (unsign)
		hd_info(CYAN("unsigned = "), "true");
	else
		hd_info(CYAN("unsigned = "), "false");
	if (live_mode) {
		hd_info(CYAN("min_pkt  = "), min_packets);
		hd_info(CYAN("max_pkt  = "), max_packets);
	}
	hd_info(CYAN("workers = "), workers);
	if (this->output_index not_eq -1) {
		hd_info(CYAN("index    = "), this->index_map[output_index]);
	}
	if (duration > 0) {
		hd_info(CYAN("duration = "), duration, "秒");
	}
	if (send_kafka and not kafka_config.empty()) {
		hd_info(CYAN("config   = "), kafka_config);
	}
	if (this->write_file and not out_path.empty()) {
		hd_info(CYAN("output   = "), out_path);
	}
	if (this->offline_mode and not pcap_file.empty()) {
		hd_info(CYAN("pcap_file = "), pcap_file);
	}
	if (not device.empty()) {
		hd_info(CYAN("device    = "), device);
	}
}
