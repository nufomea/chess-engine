## Prefile Result

2회 진행하였으며, 기술된 값은 평균값임.

### Performance Snapshot
<pre>
IPC: 3.643
</pre>

#### P-core
<pre>
Retiring: 67.8%
Front-End Bound: 19.2%
Bad Speculation: 6.5%
Back-End Bound: 6.4%
</pre>

#### E-core
<pre>
Retiring: 7.1%
Front-End Bound: 22.6%
Bad Speculation: 38.7% (18.8 + 58.7)
Back-End Bound: 31.5% (43.3 + 19.7)
</pre>

이하는 1회 진행.

### Hotspots
<pre>
Elapsed Time: 31.598s
CPU Time: 30.959s
</pre>

|Function|CPU Time|% of CPU Time|
|:---:|:---:|:---:|
|Board::push_impl<1>|12.943s|41.8%|
|perft|6.022s|19.5%|
|Board::pop|4.551s|14.7%|
|Board::get_legal_moves_impl<1>|2.803s|9.1%|

### Memory Access
<pre>
Elapsed Time: 33.891s
CPU Time: 31.746s

P-core Memory Bound: 0.3%
L1 Bound: 1.4%
L2 Bound: 0.1%
L3 Bound: 0.1%

E-core Memory Bound: 0.0%

Loads: 217,527,525,630
Stores: 130,147,704,314
</pre>

## 성능 평가

### Temp 1
<pre>
Visited nodes: 3195901860
Time elapsed : 31.3298 s
NPS          : 102008487 nodes/s
</pre>

### Temp 2
<pre>
Visited nodes: 3195901860
Time elapsed : 31.1379 s
NPS          : 102637010 nodes/s
</pre>

### Temp 3
<pre>
Visited nodes: 3195901860
Time elapsed : 31.1487 s
NPS          : 102601538 nodes/s
</pre>

### Temp 4
<pre>
Visited nodes: 3195901860
Time elapsed : 31.2528 s
NPS          : 102259805 nodes/s
</pre>

### Temp 5
<pre>
Visited nodes: 3195901860
Time elapsed : 31.5461 s
NPS          : 101308953 nodes/s
</pre>
