# transducer simulation, computing lexicographically least output
import sys
import time

def trans(T): # map-version of transitions
    trans = {}
    for (q, i, o, p) in T:
        if (q, i) in trans:
            trans[q,i].add((p,o))
        else: trans[q,i] = { (p,o) }
    for k in trans:
        trans[k] = elim(sorted(trans[k]))
    return trans

def elim(Q): # returns (Q x Gamma) list, with no duplicate states
    q = None
    P = []
    for (p,o) in Q:
        if p != q: P.append((p,o))
        q = p
    return P

def eps_closure(T,V):
    visited = set()
    W = []
    def clos(p,o):
        if p not in visited:
            visited.add(p)
            W.append((p,o))
            for (q,o2) in T.get((p,None),[]):
                clos(q, o + o2)
    for (p,o) in V:
        clos(p,o)
    return W

def step(T,V,a):
    visited = set()
    W = []
    for (p,o) in V:
        for (q,o2) in T.get((p,a),[]):
            if q not in visited:
                visited.add(q)
                W.append((q, o + o2))
    return W

def prefix(V):
    pref = ''
    while True:
        if len(V) == 0:
          sys.stdout.write('W has length 0')
        o1 = V[0][1]
        if len(o1)==0:
            return (pref, V)
        a = o1[0]
        V2 = []
        for (q,o) in V:
            if len(o) == 0 or o[0] != a:
                return (pref, V)
            V2.append((q,o[1:]))
        pref, V = pref + a, V2

def sim((T,q0,qf),s):
    T = trans(T)
    p, V = prefix(eps_closure(T, [(q0,'')]))
    #sys.stdout.write(p),
    for a in s:
        p, V = prefix(eps_closure(T, step(T,V,a)))
        #sys.stdout.write(p)
    for (q,o) in V:
        if q==qf:
            #print o
            return True
    return False

    
#start af kode
myTrans = {(0, None, '', 1), (1, None, '0', 2), (1, None, '1', 4), (2, 'a', '', 3), (3,None,'',1)}

myTrans2 = {(1, None, '', 2), (2, None, '0', 3), (3, 'a', '', 4), (4, None, '', 5), (5, None, '0', 6), (6, 'a', '', 7), (7, None, '', 5), (5, None, '1', 8), (8, 'b', '', 9), (9, None, '', 2), (2, None, '1', 10)}
myTrans3 = {(1, None, '0', 2), (2, None, '', 3), (3, None, '0', 4), (4, 'a', '', 5), (5, None, '', 3), (1, None, '1', 7), (7, None, '', 8), (8, None, '0', 9), (9, 'a', '', 10), (10, None, '', 8), (3, None, '1', 6), (8, None, '1', 11), (6, None, '', 12), (11, None, '', 12)}

f = open('myTest', 'r')
myS = f.read()
f.close()

'''
start = time.time()
sim((myTrans, 1, 10), myS)
end = time.time()

print end - start'''

myInput = 'a'*200000

start = time.time()
sim((myTrans3, 1, 12), myInput)
end = time.time()

print end - start