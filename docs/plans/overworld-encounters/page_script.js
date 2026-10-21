const BEH=[
 {k:'guarda',label:'guarda',c:'--t-placido',d:'resta fermo e ti segue con lo sguardo'},
 {k:'gira',label:'gira',c:'--t-neutro',d:'cammina a caso, ti ignora'},
 {k:'avvicina',label:'si avvicina',c:'--t-curioso',d:'viene fino a 2 caselle, con il «?», e mantiene la distanza'},
 {k:'insegue',label:'insegue',c:'--t-aggressivo',d:'ti corre addosso: la lotta parte al contatto'},
 {k:'scappa',label:'scappa',c:'--t-timido',d:'si allontana camminando; se resta bloccato sparisce'},
 {k:'sparisce',label:'sparisce',c:'--t-sfuggente',d:'svanisce appena ti nota'}];
const PROF=[
 {k:'NEUTRO',c:'--t-neutro',d:'Pesi base. Un po\' di tutto.',w:{guarda:30,gira:40,avvicina:0,insegue:15,scappa:15,sparisce:0}},
 {k:'TIMIDO',c:'--t-timido',d:'Di solito si allontana.',w:{guarda:15,gira:25,avvicina:0,insegue:0,scappa:60,sparisce:0}},
 {k:'AGGRESSIVO',c:'--t-aggressivo',d:'Di solito ti insegue.',w:{guarda:15,gira:25,avvicina:0,insegue:55,scappa:5,sparisce:0}},
 {k:'CURIOSO',c:'--t-curioso',d:'Si avvicina per guardarti.',w:{guarda:20,gira:20,avvicina:45,insegue:10,scappa:5,sparisce:0}},
 {k:'PLACIDO',c:'--t-placido',d:'Quasi sempre fermo.',w:{guarda:65,gira:25,avvicina:0,insegue:5,scappa:5,sparisce:0}},
 {k:'SFUGGENTE',c:'--t-sfuggente',d:'Raro da avvicinare: sparisce o scappa.',w:{guarda:15,gira:15,avvicina:0,insegue:0,scappa:30,sparisce:40}}];
const REG={1:'Famiglie di Kanto',2:'Famiglie di Johto',3:'Famiglie di Hoenn'};
const $=s=>document.querySelector(s);
const el=(t,a={},...ch)=>{const e=document.createElement(t);for(const[k,v]of Object.entries(a)){if(k==='class')e.className=v;else if(k==='style')e.style.cssText=v;else if(k.startsWith('on'))e.addEventListener(k.slice(2),v);else if(v!==false&&v!=null)e.setAttribute(k,v===true?'':v)}for(const c of ch.flat()){if(c==null)continue;e.append(c.nodeType?c:document.createTextNode(c))}return e};
const profColor=k=>`var(${(PROF.find(p=>p.k===k)||PROF[0]).c})`;

const state={profiles:Object.fromEntries(PROF.map(p=>[p.k,{...p.w}])),fam:{},gen:'all',q:'',onlyOpen:false};
for(const f of DATA)state.fam[f.id]={profile:f.prop,species:Object.fromEntries(f.species.filter(s=>s.prop).map(s=>[s.id,s.prop])),reviewed:false,note:''};

let db=null;const timers={};
function setStatus(){
  const n=DATA.length,rev=DATA.filter(f=>state.fam[f.id].reviewed).length,ch=DATA.filter(f=>isChanged(f)).length;
  $('#status').replaceChildren(el('span',{},'Rivedute ',el('b',{},`${rev}/${n}`)),el('span',{},'Cambiate rispetto alla proposta ',el('b',{},String(ch))),el('span',{},db?'Salvataggio attivo: le scelte arrivano a Claude.':'Non salvato: le modifiche si perdono chiudendo la pagina.'));
}
function isChanged(f){const s=state.fam[f.id];if(s.profile!==f.prop)return true;const prop=Object.fromEntries(f.species.filter(x=>x.prop).map(x=>[x.id,x.prop]));const a=JSON.stringify(Object.entries(s.species).sort()),b=JSON.stringify(Object.entries(prop).sort());return a!==b}
function save(path,data){if(!db)return;clearTimeout(timers[path]);timers[path]=setTimeout(()=>{db.doc(path).set(data).catch(e=>showBanner('Salvataggio non riuscito ('+(e&&e.code||'errore')+'). Ricarica la pagina e riprova.'))},500)}
function showBanner(t){const b=$('#banner');b.textContent=t;b.hidden=false}
function saveFam(id){save('families/'+id,{...state.fam[id]});setStatus()}

function profileSelect(value,withInherit,onchange,label){
  const s=el('select',{'aria-label':label,onchange:e=>onchange(e.target.value)});
  if(withInherit)s.append(el('option',{value:''},'= famiglia'));
  for(const p of PROF)s.append(el('option',{value:p.k},p.k));
  s.value=value||'';return s;
}

function renderLegend(){$('#legend').replaceChildren(...BEH.map(b=>el('span',{},el('i',{style:`background:var(${b.c})`}),el('b',{},b.label),' — ',b.d)))}
function bar(w){const tot=BEH.reduce((a,b)=>a+(+w[b.k]||0),0)||1;return el('div',{class:'bar',role:'img','aria-label':BEH.map(b=>`${b.label} ${Math.round((+w[b.k]||0)*100/tot)}%`).join(', ')},...BEH.map(b=>el('span',{style:`width:${(+w[b.k]||0)*100/tot}%;background:var(${b.c})`})))}
function renderProfiles(){
  $('#profiles').replaceChildren(...PROF.map(p=>{
    const w=state.profiles[p.k];const used=DATA.filter(f=>state.fam[f.id].profile===p.k).length;
    const barHost=el('div',{},bar(w));
    const pct=el('div',{class:'desc'});const upd=()=>{const tot=BEH.reduce((a,b)=>a+(+w[b.k]||0),0)||1;pct.textContent=BEH.filter(b=>+w[b.k]).map(b=>`${b.label} ${Math.round(w[b.k]*100/tot)}%`).join(' · ')};upd();
    return el('div',{class:'prof'},
      el('header',{},el('span',{class:'chip',style:`color:${profColor(p.k)}`},p.k),el('span',{class:'desc'},`${p.d} ${used} famiglie.`)),
      barHost,pct,
      el('div',{class:'weights'},...BEH.map(b=>el('label',{},b.label,el('input',{type:'number',min:'0',max:'100',step:'5',id:`w-${p.k}-${b.k}`,value:String(w[b.k]),oninput:e=>{w[b.k]=Math.max(0,Math.min(100,+e.target.value||0));barHost.replaceChildren(bar(w));upd();save('config/profiles',state.profiles)}})))));
  }));
}

function famRow(f){
  const s=state.fam[f.id];
  const row=el('div',{class:'fam',id:'f-'+f.id,style:`--stripe:${profColor(s.profile)}`});
  const badges=el('span',{style:'display:flex;gap:6px;flex-wrap:wrap'});
  const paint=()=>{row.style.setProperty('--stripe',profColor(s.profile));badges.replaceChildren(...[
    el('span',{class:'badge'},'proposta: '+f.prop),
    f.mod?el('span',{class:'badge',title:'Valori ripresi dal mod Wilds of Kanto'},'dal mod'):null,
    isChanged(f)?el('span',{class:'badge changed'},'cambiata'):null].filter(Boolean))};
  const main=profileSelect(s.profile,false,v=>{s.profile=v;paint();saveFam(f.id);renderProfiles()},'Indole della famiglia '+f.name);main.classList.add('main');main.id='sel-'+f.id;
  const rev=el('input',{type:'checkbox',id:'rev-'+f.id,onchange:e=>{s.reviewed=e.target.checked;saveFam(f.id);applyFilter()}});rev.checked=s.reviewed;
  const spRows=f.species.map(sp=>{
    const sel=profileSelect(s.species[sp.id]||'',true,v=>{if(v)s.species[sp.id]=v;else delete s.species[sp.id];paint();saveFam(f.id)},'Indole di '+sp.name);sel.id='sp-'+sp.id;
    return el('div',{class:'sp'},el('div',{style:'min-width:0'},el('b',{},sp.name),' ',el('span',{class:'badge'},sp.kinds.join(' + ')),el('div',{class:'maps'},sp.maps.join(', '))),sel)});
  const note=el('input',{type:'text',id:'note-'+f.id,placeholder:'Nota per Claude (facoltativa)',value:s.note,oninput:e=>{s.note=e.target.value;saveFam(f.id)}});
  paint();
  row.append(
    el('div',{style:'min-width:0'},el('div',{class:'top'},el('span',{class:'name'},f.name),badges),el('p',{class:'why'},f.why)),
    el('div',{class:'ctl'},main,el('label',{class:'tog'},rev,'rivista')),
    el('details',{},el('summary',{},`${f.species.length} specie · ${[...new Set(f.species.flatMap(x=>x.maps))].length} mappe`),el('div',{class:'species'},spRows)),
    el('div',{class:'note'},note));
  row._paint=()=>{main.value=s.profile;rev.checked=s.reviewed;if(document.activeElement!==note)note.value=s.note;f.species.forEach(sp=>{const x=row.querySelector('#sp-'+CSS.escape(sp.id));if(x)x.value=s.species[sp.id]||''});paint()};
  row._text=(f.name+' '+f.species.map(x=>x.name+' '+x.maps.join(' ')).join(' ')).toLowerCase();
  return row;
}
const rows={};
function renderList(){
  const list=$('#list');list.replaceChildren();
  for(const g of [1,2,3]){
    const fams=DATA.filter(f=>f.gen===g);
    const box=el('div',{class:'rows'},...fams.map(f=>rows[f.id]=famRow(f)));
    list.append(el('section',{class:'region','data-gen':g},el('h2',{},REG[g],el('small',{},`${fams.length} famiglie · ${fams.reduce((a,f)=>a+f.species.length,0)} specie`)),box));
  }
  list.append(el('p',{class:'empty',id:'none',hidden:true},'Nessuna famiglia corrisponde. Svuota la ricerca o togli "solo da rivedere".'));
}
function applyFilter(){
  let any=false;
  document.querySelectorAll('.region').forEach(r=>{const g=+r.dataset.gen;let vis=0;
    DATA.filter(f=>f.gen===g).forEach(f=>{const ok=(state.gen==='all'||state.gen==g)&&(!state.q||rows[f.id]._text.includes(state.q))&&(!state.onlyOpen||!state.fam[f.id].reviewed);rows[f.id].hidden=!ok;if(ok)vis++});
    r.hidden=!vis;if(vis)any=true});
  $('#none').hidden=any;setStatus();
}
function renderSeg(){const opts=[['all','Tutte'],['1','Kanto'],['2','Johto'],['3','Hoenn']];$('#genseg').replaceChildren(...opts.map(([k,l])=>el('button',{type:'button','aria-pressed':String(state.gen==k),onclick:()=>{state.gen=k;renderSeg();applyFilter()}},l)))}

renderLegend();renderProfiles();renderSeg();renderList();applyFilter();
$('#q').addEventListener('input',e=>{state.q=e.target.value.trim().toLowerCase();applyFilter()});
$('#onlyOpen').addEventListener('change',e=>{state.onlyOpen=e.target.checked;applyFilter()});

(async()=>{
  const c=window.claude;db=c&&c.use?await c.use('db'):null;
  if(!db){showBanner('Il salvataggio non è disponibile in questa vista (serve aver fatto l\'accesso a claude.ai). Puoi guardare la tabella, ma le scelte non verranno salvate.');setStatus();return}
  setStatus();
  db.doc('config/profiles').onSnapshot(snap=>{const d=snap.exists&&snap.data();if(!d)return;for(const p of PROF)if(d[p.k])state.profiles[p.k]={...state.profiles[p.k],...d[p.k]};if(!document.activeElement||!document.activeElement.id.startsWith('w-'))renderProfiles()});
  db.collection('families').onSnapshot(snap=>{for(const d of snap.docs){const v=d.data();if(!v||!state.fam[d.id])continue;Object.assign(state.fam[d.id],{profile:v.profile||state.fam[d.id].profile,species:v.species||{},reviewed:!!v.reviewed,note:v.note||''});rows[d.id]&&rows[d.id]._paint()}renderProfiles();applyFilter()},e=>showBanner('Lettura del salvataggio non riuscita ('+(e&&e.code)+').'));
})();
