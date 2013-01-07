/**
 * address_histogram.cpp: 
 * Show packets received vs port
 *
 * This source file is public domain, as it is not based on the original tcpflow.
 *
 * Author: Michael Shick <mike@shick.in>
 *
 */

#include "config.h"
#include "tcpflow.h"
#include "tcpip.h"

#include <iomanip>

#include "address_histogram.h"

void address_histogram::ingest_packet(const packet_info &pi)
{
    struct ip4_dgram ip4;
    struct ip6_dgram ip6;
    std::stringstream ss;

    // IPv4?
    if(tcpip::ip4_from_bytes(pi.data, pi.caplen, ip4)) {
        // recording source addresses?
        if(relationship == SENDER || relationship == SND_OR_RCV) {
            parent_count_histogram.increment(std::string(inet_ntoa(ip4.header->ip_src)), 1);
        }
        if(relationship == RECEIVER || relationship == SND_OR_RCV) {
            parent_count_histogram.increment(std::string(inet_ntoa(ip4.header->ip_dst)), 1);
        }
    }
    // IPv6?
    else if(tcpip::ip6_from_bytes(pi.data, pi.caplen, ip6)) {
        ss.str(std::string());
        ss << std::hex << std::setfill('0');
        // recording source addresses?
        if(relationship == SENDER || relationship == SND_OR_RCV) {
            for(int ii = 0; ii < 8; ii++) {
                ss << std::setw(4) << ip6.header->ip6_src.__u6_addr.__u6_addr8[ii];
                if(ii < 7) {
                    ss << ":";
                }
            }
            parent_count_histogram.increment(ss.str(), 1);
            ss.str(std::string());
            ss << std::hex << std::setfill('0');
        }
        // recording destination addresses?
        if(relationship == RECEIVER || relationship == SND_OR_RCV) {
            for(int ii = 0; ii < 8; ii++) {
                ss << std::setw(4) << ip6.header->ip6_dst.__u6_addr.__u6_addr8[ii];
                if(ii < 7) {
                    ss << ":";
                }
            }
            parent_count_histogram.increment(ss.str(), 1);
        }
    }
}

void address_histogram::render(cairo_t *cr, const plot::bounds_t &bounds)
{
#ifdef CAIRO_PDF_AVAILABLE
    parent_count_histogram.render(cr, bounds);
#endif
}
