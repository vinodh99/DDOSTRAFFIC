#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/lte-global-pathloss-database.h"
#include "ns3/config-store.h"
#include <cmath>
#include "ns3/gnuplot.h"
 #include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <cassert>
#include <fstream>
#include <stdio.h>  
#include <stdlib.h>



using namespace ns3;
using namespace std;
 
NS_LOG_COMPONENT_DEFINE ("DOS");

uint32_t pkz=512;





 
void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
  nodeA->GetObject<Ipv4> ()->SetDown (interfaceA);
  nodeB->GetObject<Ipv4> ()->SetDown (interfaceB);
}


 
 

int main (int argc, char **argv)
{
  bool verbose = false;
  bool printRoutingTables = true;
  bool showPings = false;

int numnodes=200; //number of nodes

  std::string SplitHorizon ("SplitHorizon");
 double simTime=60.0;
  CommandLine cmd;
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("printRoutingTables", "Print routing tables at 30, 60 and 90 seconds", printRoutingTables);
  cmd.AddValue ("showPings", "Show Ping6 reception", showPings);
  cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon, SplitHorizon, PoisonReverse)", SplitHorizon);
  cmd.Parse (argc, argv);
 
  if (verbose)
    {
      LogComponentEnableAll (LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE));
      LogComponentEnable ("RipSimpleRouting", LOG_LEVEL_INFO);
      LogComponentEnable ("Rip", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4Interface", LOG_LEVEL_ALL);
      LogComponentEnable ("Icmpv4L4Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("ArpCache", LOG_LEVEL_ALL);
      LogComponentEnable ("V4Ping", LOG_LEVEL_ALL);
    }
 
  if (SplitHorizon == "NoSplitHorizon")
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::NO_SPLIT_HORIZON));
    }
  else if (SplitHorizon == "SplitHorizon")
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::SPLIT_HORIZON));
    }
  else
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));
    }
 NodeContainer gnode,gnode1;
gnode.Create(numnodes/2);
gnode1.Create(numnodes/2);

  NS_LOG_INFO ("Create nodes.");
  Ptr<Node> server = CreateObject<Node> ();
  Names::Add ("ServerNode", server);
  Ptr<Node> R1 = CreateObject<Node> ();
  Names::Add ("zombie1", R1);
  Ptr<Node> R2 = CreateObject<Node> ();
  Names::Add ("zombie 2", R2);
 
  Ptr<Node> atttack1= CreateObject<Node> ();
  Names::Add ("Attacker1", atttack1);
  Ptr<Node> atttack2= CreateObject<Node> ();
  Names::Add ("Attacker2", atttack2);
  Ptr<Node> atttack3= CreateObject<Node> ();
  Names::Add ("Attacker3", atttack3);
 
  NodeContainer net1 (server, R1);
  NodeContainer net2 (R1, R2);
  NodeContainer net3 (R2, gnode);
  NodeContainer net4 (R2, gnode1);
  NodeContainer net5 (R2, gnode1);
  NodeContainer net6 (R1, atttack1);
  NodeContainer net7 (R1, atttack2);
  NodeContainer net8 (R2, atttack3);
 
 
  NodeContainer routers (R1, R2);
  NodeContainer nodes (server, gnode, gnode1);
  NodeContainer goodNodes (gnode,gnode1);
  NodeContainer AttackNodes (atttack1,atttack2,atttack3);
   
 
 
 
  NS_LOG_INFO ("Create channels.");
  CsmaHelper csmaG, csmaE, csmaR;
  csmaR.SetChannelAttribute ("DataRate", StringValue ("200Mbps"));
  csmaR.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (1)));
 
  csmaG.SetChannelAttribute ("DataRate", StringValue ("60Mbps"));
  csmaG.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
 
  csmaE.SetChannelAttribute ("DataRate", StringValue ("80Mbps"));
  csmaE.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
 
  NetDeviceContainer ndc1 = csmaR.Install (net1);
  NetDeviceContainer ndc2 = csmaR.Install (net2);
  NetDeviceContainer ndc3 = csmaG.Install (net3);
  NetDeviceContainer ndc4 = csmaG.Install (net4);
  NetDeviceContainer ndc5 = csmaG.Install (net5);
  NetDeviceContainer ndc6 = csmaE.Install (net6);
  NetDeviceContainer ndc7 = csmaE.Install (net7);
  NetDeviceContainer ndc8 = csmaE.Install (net8);
 
  NS_LOG_INFO ("Create IPv4 and routing");
  RipHelper ripRouting;
 
 
  ripRouting.ExcludeInterface (R1, 1);
  ripRouting.ExcludeInterface (R1, 3);
  ripRouting.ExcludeInterface (R1, 4);
  ripRouting.ExcludeInterface (R2, 2);
  ripRouting.ExcludeInterface (R2, 3);
  ripRouting.ExcludeInterface (R2, 4);
  ripRouting.ExcludeInterface (R2, 5);
   
 
  ripRouting.SetInterfaceMetric (R1, 1, 8);
  ripRouting.SetInterfaceMetric (R1, 2, 10);
  ripRouting.SetInterfaceMetric (R1, 3, 10);
 
   
 
  Ipv4ListRoutingHelper listRH;
  listRH.Add (ripRouting, 0);
//  Ipv4StaticRoutingHelper staticRh;
//  listRH.Add (staticRh, 5);
 
  InternetStackHelper internet;
  internet.SetIpv6StackInstall (false);
  internet.SetRoutingHelper (listRH);
  internet.Install (routers);
 
  InternetStackHelper internetNodes;
  internetNodes.SetIpv6StackInstall (false);
  internetNodes.Install (nodes);
  internetNodes.Install (AttackNodes);

 

  NS_LOG_INFO ("Assign IPv4 Addresses.");
  Ipv4AddressHelper ipv4;
 
  ipv4.SetBase (Ipv4Address ("10.0.0.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic1 = ipv4.Assign (ndc1);
 
  ipv4.SetBase (Ipv4Address ("10.0.1.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic2 = ipv4.Assign (ndc2);
 
  ipv4.SetBase (Ipv4Address ("10.0.2.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic3 = ipv4.Assign (ndc3);
 
  ipv4.SetBase (Ipv4Address ("10.0.3.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic4 = ipv4.Assign (ndc4);
 
  ipv4.SetBase (Ipv4Address ("10.0.4.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic5 = ipv4.Assign (ndc5);
 
  ipv4.SetBase (Ipv4Address ("10.0.5.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic6 = ipv4.Assign (ndc6);
 
  ipv4.SetBase (Ipv4Address ("10.0.6.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic7 = ipv4.Assign (ndc7);
 
  ipv4.SetBase (Ipv4Address ("10.0.7.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic8 = ipv4.Assign (ndc8);
 
 
  Ptr<Ipv4StaticRouting> staticRouting;
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (server->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.0.2", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (gnode.Get(0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.2.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (gnode1.Get(0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.3.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (gnode1.Get(0)->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.4.1", 1 );
 staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atttack1->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.5.1", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atttack2->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.6.1", 1 );
 staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (atttack3->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.7.1", 1 );
 
 
  if (printRoutingTables)
    {
      RipHelper routingHelper;
 
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);
 
      routingHelper.PrintRoutingTableAt (Seconds (50.0), R1, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (50.0), R2, routingStream);
     
      routingHelper.PrintRoutingTableAt (Seconds (20.0), R1, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (20.0), R2, routingStream);
     
      
      }
 
  NS_LOG_INFO ("Create Applications.");
Address serverAddress = Address(iic1.GetAddress(0));
 /*
    uint16_t port = 4000;
    UdpServerHelper serverHelper (port);
    ApplicationContainer apps = serverHelper.Install (server);
    apps.Start (Seconds (5.0));
    apps.Stop (Seconds (simTime));
 
 

    
    uint32_t MaxPacketSize = 1024;
    Time interPacketInterval = Seconds (3); //20 packets per second
    uint32_t maxPacketCount = 1200;
    UdpClientHelper client (serverAddress, port);
    client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    client.SetAttribute ("Interval", TimeValue (interPacketInterval));
    client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
    apps = client.Install (g1);
    apps.Start (Seconds (5.0));
    apps.Stop (Seconds (simTime));
 
    apps = client.Install (g2);
    apps.Start (Seconds (10.0));
    apps.Stop (Seconds (simTime));
 
    apps = client.Install (g3);
    apps.Start (Seconds (20.0));
    apps.Stop (Seconds (simTime));
 
    Time interPacketIntervalB1 = Seconds (0.75);
    Time interPacketIntervalB = Seconds (1);
    uint32_t maxPacketCountB = 12000;
    UdpClientHelper clientB (serverAddress, port);
    clientB.SetAttribute ("MaxPackets", UintegerValue (maxPacketCountB));
    clientB.SetAttribute ("Interval", TimeValue (interPacketIntervalB));
    clientB.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
    apps = clientB.Install (atttack1);
    apps.Start (Seconds (2.0));
    apps.Stop (Seconds (3.0));
 
    apps = clientB.Install (atttack2);
    apps.Start (Seconds (3.0));
    apps.Stop (Seconds (4.0));
    
    UdpClientHelper clientB1 (serverAddress, port);
    clientB1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCountB));
    clientB1.SetAttribute ("Interval", TimeValue (interPacketIntervalB1));
    clientB1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
    
    apps = clientB1.Install (atttack3);
    apps.Start (Seconds (4.0));
    apps.Stop (Seconds (5.0));
 
 */


 
   uint16_t httpServerPort = 80;
  ApplicationContainer httpServerApps;
  ApplicationContainer httpClientApps;

  HttpServerHelper httpServer (httpServerPort);
  httpServerApps.Add (httpServer.Install (server));

  HttpClientHelper httpClient (serverAddress, httpServerPort);
  httpClientApps.Add (httpClient.Install (atttack1));

 
 
 
 
  AsciiTraceHelper ascii;
  csmaR.EnableAsciiAll (ascii.CreateFileStream ("dos-attack.tr"));
  csmaR.EnablePcapAll ("dos-attack", true);
 
 NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (80.0));
 
 
 Ptr<FlowMonitor> flowMonitor;
     FlowMonitorHelper flowHelper;
      flowMonitor = flowHelper.InstallAll ();
    
 
  Simulator::Run ();
  flowMonitor->SerializeToXmlFile("dosFlowMonitor.xml", true, true);

//===========================================

flowMonitor->CheckForLostPackets(Time(Seconds(2)));  // packet is assumed lost if not received in 2 sec
     Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
     map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();

         double DLAvDelay=0;
      
         double DLjitter=0;
         double Laten=0;
       
         int dlCount = 0;     
     

        
         uint32_t rxBytessumDL = 0;
        
   	  uint32_t lostPacketsumDL = 0;
   	
  
		 for (map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
		   {
		  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

			
					 lostPacketsumDL += i->second.lostPackets;
					 rxBytessumDL+=i->second.rxBytes;

				  if(i->second.rxPackets!=0) {

					   dlCount++;

					   cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
					   cout << " F-LBT TX packets: " << i->second.txPackets  <<"\n";
					   cout << " F-LBT RX packets: " << i->second.rxPackets  <<"\n";
					   cout << " DL Delay: " << i->second.delaySum.GetSeconds()/i->second.rxPackets  <<"\n";
					   DLAvDelay=DLAvDelay+i->second.delaySum.GetSeconds()/i->second.rxPackets;
                                      //  DLjitter=i->second.jitterSum.GetSeconds()/(i->second.rxPackets-1);
                                          Laten=Laten+(DLAvDelay/100.0);

					   if(i->second.rxPackets > 1){

								   DLjitter=DLjitter+i->second.jitterSum.GetSeconds()/(i->second.rxPackets-1);

							  }



						}

			

			 
				

			   



			}

NS_LOG_UNCOND("---------- AVERAGE ---------");

		  cout << " Delay: " << DLAvDelay*1000/dlCount << " ms\n";
		  cout << "jitter: " << DLjitter*1000/dlCount << " ms  \n";
                  cout << "latency: " << Laten*1000/dlCount << " ms  \n";

		

			
		
		  cout << " Throughput : " << rxBytessumDL*8/1024/(simTime-0.1) << " Kbps \n";

//============================================

  Simulator::Destroy ();



std::ofstream ofs ("Throughput.plt", std::ofstream::out);
 ofs << "set terminal png"<<std::endl;
 ofs << "set output 'Throughput.png'"<<std::endl;
  ofs << "set title ''"<<std::endl;
  ofs << "set xlabel 'Time(sec)' "<<std::endl;
  ofs << "set ylabel 'Throughput' "<<std::endl;
  ofs << "plot "<<" '-'" <<"title "<<"'Throughput(Kbps)'  with linespoints" <<std::endl;
 ofs <<"0 " <<1<<std::endl;
 ofs <<simTime<<" " <<rxBytessumDL*8/1024/(60-0.1)<<std::endl;
ofs << "e"<<std::endl;
  ofs.close();

//=====================

std::ofstream ofs1 ("graph.plt", std::ofstream::out);
 ofs1 << "set terminal png"<<std::endl;
 ofs1 << "set output 'graph.png'"<<std::endl;
  ofs1 << "set title ''"<<std::endl;
  ofs1 << "set xlabel 'Time(sec)' "<<std::endl;
  ofs1 << "set ylabel 'value(ms)' "<<std::endl;
  ofs1 << "plot "<<" '-'" <<"title "<<"'delay'  with linespoints,'-' title 'latency'  with linespoints,'-' title 'jitter'  with linespoints " <<std::endl;
 ofs1 <<"0 " <<0.1<<std::endl;
 ofs1 <<simTime<<" " <<(DLAvDelay*1000/dlCount)<<std::endl;
ofs1 << "e"<<std::endl;


 ofs1 <<"0 " <<0.1<<std::endl;
 ofs1 <<simTime<<" " <<(Laten*1000/dlCount)<<std::endl;
ofs1 << "e"<<std::endl;


 ofs1 <<"0 " <<0.1<<std::endl;
 ofs1 <<simTime<<" " <<(DLjitter*1000/dlCount)<<std::endl;
ofs1 << "e"<<std::endl;
  ofs1.close();

system("gnuplot 'Throughput.plt'");
system("gnuplot 'graph.plt'");

  NS_LOG_INFO ("Done.");
}
