import re, subprocess, sys
OBJ='build/hns/src/party_menu.o'
out=subprocess.run(["arm-none-eabi-nm","--defined-only",OBJ],capture_output=True,text=True).stdout
PREFIXES=("ClassicPartyMenu_","SwShPartyMenu_")
def strip(n):
    for p in PREFIXES:
        if n.startswith(p): return n[len(p):]
    return n
funcs,data=[],[]
for l in out.split("\n"):
    p=l.split()
    if len(p)==3 and p[1].isupper():
        (funcs if p[1]=="T" else data).append(strip(p[2]))
SHARED={"gPartyMenu","gPartyMenuUseExitCallback","gSelectedMonPartyId","gPostMenuFieldCallback",
        "gSelectedOrderFromParty","gBattlePartyCurrentOrder","gItemUseCB"}
NOT_API={"IsFusionMon"}
funcs=sorted(set(f for f in funcs if f not in NOT_API))
assets=sorted(set(d for d in data if d not in SHARED))
src=open('src/party_menu.c').read()
rows=[]
for f in funcs:
    m=re.search(r"^([A-Za-z_][\w \*]*?)\b%s\s*\(([^;{]*)\)\s*\{" % re.escape(f), src, re.M)
    if not m:
        print("UNRESOLVED:", f, file=sys.stderr); sys.exit(1)
    ret=m.group(1).strip(); params=m.group(2).strip()
    if params in ("","void"): args=""; params="void"
    else:
        depth=0;cur="";parts=[]
        for ch in params:
            if ch in "([": depth+=1
            elif ch in ")]": depth-=1
            if ch=="," and depth==0: parts.append(cur); cur=""
            else: cur+=ch
        parts.append(cur)
        args=", ".join((re.findall(r"(\w+)\s*(?:\[\s*\])?\s*$",p.strip()) or ["?"])[0] for p in parts)
    rows.append(("DISPATCH_VOID" if ret=="void" else "DISPATCH_RET", ret, f, params, args))
print("funcs",len(funcs),"assets",len(assets),"rows",len(rows))
import json; json.dump({"funcs":funcs,"assets":assets,"rows":rows}, open('/tmp/variant.json','w'))
