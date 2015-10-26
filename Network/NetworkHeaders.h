#ifndef NETWORKHEADERS_H
#define NETWORKHEADERS_H
#include <hw_address.h>
#include <endianess.h>
#include <Utils\macros.h>
namespace Ace {
	namespace Network {
		
		ACE_BEGIN_PACK
		struct arphdr {
			typedef HWAddress<6> hwaddress_type;
			uint16_t ar_hrd;	/* format of hardware address	*/
			uint16_t ar_pro;	/* format of protocol address	*/
			uint8_t	ar_hln;		/* length of hardware address	*/
			uint8_t	ar_pln;		/* length of protocol address	*/
			uint16_t ar_op;		/* ARP opcode (command)		*/

			/* sender hardware address	*/
			uint8_t ar_sha[hwaddress_type::address_size];
			/* sender IP address		*/
			uint32_t ar_sip;
			/* target hardware address	*/
			uint8_t ar_tha[hwaddress_type::address_size];
			/* target IP address		*/
			uint32_t ar_tip;
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct dnshdr {
			uint16_t id;
#if ACE_IS_LITTLE_ENDIAN
			uint16_t
			rd : 1,
			 tc : 1,
			  aa : 1,
			   opcode : 4,
					qr : 1,
					 rcode : 4,
						 cd : 1,
						  ad : 1,
						   z : 1,
						   ra : 1;
#elif ACE_IS_BIG_ENDIAN
			uint16_t
			qr : 1,
			 opcode : 4,
				  aa : 1,
				   tc : 1,
					rd : 1,
					 ra : 1,
					  z : 1,
					  ad : 1,
					   cd : 1,
						rcode : 4;
#endif
			uint16_t questions, answers,
				authority, additional;
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct ethhdr {
			typedef HWAddress<6> address_type;
			uint8_t dst_mac[address_type::address_size];
			uint8_t src_mac[address_type::address_size];
			uint16_t payload_type;
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct icmphdr {
			uint8_t	type;
			uint8_t	code;
			uint16_t check;
			union {
				struct {
					uint16_t id;
					uint16_t sequence;
				} echo;
				uint32_t gateway;
				struct {
					uint16_t unused;
					uint16_t mtu;
				} frag;
				uint8_t pointer;
			} un;
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct icmp6hdr {
			uint8_t	type;
			uint8_t code;
			uint16_t cksum;
			union {
				struct {
					uint16_t identifier;
					uint16_t sequence;
				} u_echo;

				struct {
#if ACE_IS_LITTLE_ENDIAN
					uint32_t reserved : 5,
					override : 1,
				solicited : 1,
						router : 1,
							 reserved2 : 24;
#else
					uint32_t router : 1,
				solicited : 1,
							override : 1,
						reserved : 29;
#endif						
				} u_nd_advt;
				struct {
					uint8_t	hop_limit;
#if ACE_IS_LITTLE_ENDIAN
					uint8_t reserved : 3,
					router_pref : 2,
							  home_agent : 1,
									   other : 1,
										   managed : 1;
#else
					uint8_t managed : 1,
					other : 1,
						home_agent : 1,
								 router_pref : 2,
										   reserved : 3;
#endif
					uint16_t router_lifetime;
				} u_nd_ra;
			};
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct iphdr {
#if ACE_IS_LITTLE_ENDIAN
			uint8_t ihl : 4,
		version : 4;
#else
			uint8_t version : 4,
		ihl : 4;
#endif
			uint8_t tos;
			uint16_t tot_len;
			uint16_t id;
			uint16_t frag_off;
			uint8_t ttl;
			uint8_t protocol;
			uint16_t check;
			struct  in_addr ip_src, ip_dst;  /* source and dest address */
			/*The options start here. */
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct ipv6_header {
#if ACE_IS_BIG_ENDIAN
			uint32_t version : 4,
		traffic_class : 8,
					flow_label : 20;
			uint32_t payload_length : 16,
			next_header : 8,
					  hop_limit : 8;
#else
			uint8_t traffic_class : 4,
		version : 4;
			uint8_t flow_label[3];
			uint16_t payload_length;
			uint8_t next_header;
			uint8_t hop_limit;
#endif
			uint8_t src_addr[16], dst_addr[16];
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct pppoe_hdr {
#if ACE_IS_LITTLE_ENDIAN
			uint8_t version : 4,
		type : 4;
			uint8_t code;
#else
			uint16_t version : 4,
		type : 4,
		   code : 8;
#endif
			uint16_t session_id;
			uint16_t payload_length;
		};
		ACE_END_PACK

#if ACE_IS_LITTLE_ENDIAN
			ACE_BEGIN_PACK
            struct radio_flags_type {
                uint32_t
                    tsft:1,
                    flags:1,
                    rate:1,
                    channel:1,
                    fhss:1,
                    dbm_signal:1,
                    dbm_noise:1,
                    lock_quality:1,

                    tx_attenuation:1,
                    db_tx_attenuation:1,
                    dbm_tx_power:1,
                    antenna:1,
                    db_signal:1,
                    db_noise:1,
                    rx_flags:1,
                    tx_flags:1,

                    reserved1:1,
                    data_retries:1,
                    channel_plus:1,
                    mcs:1,
                    reserved2:4,

                    reserved3:7,
                    ext:1;
        };
			ACE_END_PACK
#else
		ACE_BEGIN_PACK
		struct radio_flags_type {
                uint32_t
                    lock_quality:1,
                    dbm_noise:1,
                    dbm_signal:1,
                    fhss:1,
                    channel:1,
                    rate:1,
                    flags:1,
                    tsft:1,

                    tx_flags:1,
                    rx_flags:1,
                    db_noise:1,
                    db_signal:1,
                    antenna:1,
                    dbm_tx_power:1,
                    db_tx_attenuation:1,
                    tx_attenuation:1,

                    reserved2:4,
                    mcs:1,
                    channel_plus:1,
                    data_retries:1,
                    reserved1:1,

                    ext:1,
                    reserved3:7;
		};
		ACE_END_PACK
#endif
		ACE_BEGIN_PACK
		struct radiotap_hdr {
#if ACE_IS_LITTLE_ENDIAN
			uint8_t it_version;
			uint8_t it_pad;
#else
			uint8_t it_pad;
			uint8_t it_version;
#endif // ACE_IS_LITTLE_ENDIAN 
			uint16_t it_len;
			union {
				radio_flags_type flags;
				uint32_t flags_32;
			};
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct sllhdr {
			uint16_t packet_type, lladdr_type, lladdr_len;
			uint8_t address[8];
			uint16_t protocol;
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct pvt_bpdu_id {
#if ACE_IS_LITTLE_ENDIAN 
			// fixme
			uint16_t ext_id : 4,
		priority : 4,
			   ext_idL : 8;
#else
			uint16_t priority : 4,
		ext_id : 12;
#endif
			uint8_t id[6];
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct stphdr {
			uint16_t proto_id;
			uint8_t proto_version;
			uint8_t bpdu_type;
			uint8_t bpdu_flags;
			pvt_bpdu_id root_id;
			uint32_t root_path_cost;
			pvt_bpdu_id bridge_id;
			uint16_t port_id;
			uint16_t msg_age;
			uint16_t max_age;
			uint16_t hello_time;
			uint16_t fwd_delay;
		};
		ACE_END_PACK

#if ACE_IS_LITTLE_ENDIAN
		ACE_BEGIN_PACK
		struct tcp_flags_type {
			uint8_t fin : 1,
		syn : 1,
		  rst : 1,
			psh : 1,
			  ack : 1,
				urg : 1,
				  ece : 1,
					cwr : 1;
		} ;
		ACE_END_PACK
#else
		ACE_BEGIN_PACK
		struct tcp_flags_type {
			uint8_t cwr : 1,
		ece : 1,
		  urg : 1,
			ack : 1,
			  psh : 1,
				rst : 1,
				  syn : 1,
					fin : 1;
		} ;
		ACE_END_PACK
#endif
		ACE_BEGIN_PACK
		struct tcphdr {
			uint16_t sport;
			uint16_t dport;
			uint32_t seq;
			uint32_t ack_seq;
#if ACE_IS_LITTLE_ENDIAN
			uint8_t res1 : 4,
			doff : 4;
#else
			uint8_t doff : 4,
			res1 : 4;
#endif
			union {
				tcp_flags_type flags;
				uint8_t flags_8;
			};
			uint16_t	window;
			uint16_t	check;
			uint16_t	urg_ptr;
		};
		ACE_END_PACK

		ACE_BEGIN_PACK
		struct udphdr {
			uint16_t sport;
			uint16_t dport;
			uint16_t len;
			uint16_t check;
		};
		ACE_END_PACK
	}
}
#endif