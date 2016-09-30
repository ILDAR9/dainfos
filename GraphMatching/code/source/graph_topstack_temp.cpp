   if (find(size_list.begin(), size_list.end(), found.size()) != size_list.end()) {
                        should = true;
                        map<int, list<int>> ln;
                        map<int, list<int>> rn;
                        for (pair_iterator fit = found.begin(); fit != found.end(); fit++) {
                            ln[fit->lnode].push_back(fit->rnode);
                            rn[fit->rnode].push_back(fit->lnode);
                        }
                        list<int> key_ls = ln.begin()->second;
                        for (map<int, list<int>>::iterator it = ++ln.begin(); it != ln.end(); it++) {
                            for (list<int>::iterator lit = it->second.begin(); lit != it->second.end(); lit++) {
                                if (find(key_ls.begin(), key_ls.end(), *lit) == key_ls.end()) {
                                    should = false;
                                    goto end;
                                }
                            }
                        }
                        end:;
                        if (should){
                            key_ls = rn.begin()->second;
                            for (map<int, list<int>>::iterator it = ++rn.begin(); it != rn.end(); it++) {
                                for (list<int>::iterator lit = it->second.begin(); lit != it->second.end(); lit++) {
                                    if (find(key_ls.begin(), key_ls.end(), *lit) == key_ls.end()) {
                                        should = false;
                                        goto end2;
                                    }
                                }
                            }
                            end2:;
                        }

                    }