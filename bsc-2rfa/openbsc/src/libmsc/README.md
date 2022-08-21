# 2RFA
fork process misconfigurated\n
<p>patch osmocom-bb</p>
<p>patch openbsc</p>
<p>copy the .h Files in ~/osmocom-bb/src/host/layer23/src/mobile/ in osmocom-bb</p>
<p>copy the .h File in openbsc/src/libmsc/ in openbsc</p>
<p>invert the port between server.h and client.h 666-888</p>
<p>put your plmn imsi in ~/.osmocom/bb/mobile.cfg</p>
<p>set auth to a5 1 in openbsc.cfg</p>
<p>in terminal : </p>
   <p> telnet 0 4242 </p>
   <p> en</p>
   <p> suberiber imsi 012345678901234 xor a3a8 0123456789abcdef0123456789abcdef</p>
<p>prepare mobile</p>
<p>run openbsc </p>
<p>when phone connect whait to the server </p>
<p>quick! run mobile </p>
<p>and with wireshark can see the result</p>
