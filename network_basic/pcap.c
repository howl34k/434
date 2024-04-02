#include <stdio.h>
#include <pcap.h>
#include <netinet/ether.h>
#include <arpa/inet.h>


/* Ethernet header */
struct ethheader {
  u_char  ether_dhost[6]; /* destination host address */
  u_char  ether_shost[6]; /* source host address */
  u_short ether_type;     /* protocol type (IP, ARP, RARP, etc) */
};


/* IP Header */
struct ipheader {
  unsigned char      iph_ihl:4, //IP header length
                     iph_ver:4; //IP version
  unsigned char      iph_tos; //Type of service
  unsigned short int iph_len; //IP Packet length (data + header)
  unsigned short int iph_ident; //Identification
  unsigned short int iph_flag:3, //Fragmentation flags
                     iph_offset:13; //Flags offset
  unsigned char      iph_ttl; //Time to Live
  unsigned char      iph_protocol; //Protocol type
  unsigned short int iph_chksum; //IP datagram checksum
  struct  in_addr    iph_sourceip; //Source IP address
  struct  in_addr    iph_destip;   //Destination IP address
};


/* TCP Header */
struct tcpheader {
    u_short tcp_sport;               // source port
    u_short tcp_dport;               // destination port
    u_int   tcp_seq;                 // sequence number
    u_int   tcp_ack;                 // acknowledgement number
    u_char  tcp_offx2;               // data offset, rsvd
    u_char  tcp_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short tcp_win;                 // window
    u_short tcp_sum;                 // checksum
    u_short tcp_urp;                 // urgent pointer
};


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    struct ethheader *eth = (struct ethheader *)packet;
    printf("******************** MAC Address ********************\n");
    printf("     Source MAC Address: %s\n", ether_ntoa((struct ether_addr *)eth->ether_shost));
    printf("Destination MAC Address: %s\n", ether_ntoa((struct ether_addr *)eth->ether_dhost));
    printf("\n");

    struct ipheader * ip = (struct ipheader *)(packet + sizeof(struct ethheader)); 
    printf("******************** IP Address ********************\n");
    printf("      Source IP Address: %s\n", inet_ntoa(ip->iph_sourceip));   
    printf(" Destination IP Address: %s\n", inet_ntoa(ip->iph_destip));
    printf("\n");

    struct tcpheader * tcp = (struct tcpheader *)(packet + sizeof(struct ethheader) + sizeof(struct ipheader));
    printf("******************** Port Number ********************\n");
    printf("            Source Port: %d\n", ntohs(tcp->tcp_sport));
    printf("       Destination Port: %d\n", ntohs(tcp->tcp_dport));
    printf("\n\n\n");
}

int main(){
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Step 1: Open live pcap session on NIC with name enp0s3
    handle = pcap_open_live("ens33", BUFSIZ, 1, 1000, errbuf);

    // Step 2: Capture packets
    pcap_loop(handle, -1, got_packet, NULL);

    // Step 3: Close the handle
    pcap_close(handle);
    return 0;
}
