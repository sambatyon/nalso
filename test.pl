%my own parser
q:-p,¬ab.
q:-r,¬abp.
ab:-¬r.
abp:-¬p.%comment at the end of the line
q.
%constraints
:-p,abp.
:-r,ab.
%set of abductibles
<ab,abp,p,r>