#include <stdarg.h>
#include "../include/set.h"

jwtbundle_Set* jwtbundle_NewSet(const int n_args, ...)
{
    jwtbundle_Set *set = malloc(sizeof *set);
    mtx_init(&(set->mtx), mtx_plain);
    set->bundles = NULL;
    
    va_list args;
    va_start(args, n_args);
    
    for(int i = 0; i < n_args; ++i)
    {
        jwtbundle_Bundle *bundle = va_arg(args, jwtbundle_Bundle*);
        shput(set->bundles, bundle->td.name, bundle);
    }

    va_end(args);
    
    return set;
}

void jwtbundle_Set_Add(jwtbundle_Set *s, jwtbundle_Bundle *bundle)
{
    mtx_lock(&(s->mtx));
    shput(s->bundles, bundle->td.name, bundle);
    mtx_unlock(&(s->mtx));
}

void jwtbundle_Set_Remove(jwtbundle_Set *s, const spiffeid_TrustDomain *td)
{
    mtx_lock(&(s->mtx));
    shdel(s->bundles, td->name);
    mtx_unlock(&(s->mtx));
}

bool jwtbundle_Set_Has(jwtbundle_Set *s, const spiffeid_TrustDomain *td)
{
    mtx_lock(&(s->mtx));
    bool present = false;
    int idx = shgeti(s->bundles, td->name);
    if(idx >= 0) present = true;
    mtx_unlock(&(s->mtx));
    
    return present;
}

jwtbundle_Bundle* jwtbundle_Set_Get(jwtbundle_Set *s, 
                                    const spiffeid_TrustDomain *td, 
                                    bool *suc)
{
    mtx_lock(&(s->mtx));
    *suc = false;
    jwtbundle_Bundle *bundle = NULL;
    map_string_jwtbundle_Bundle *key_val = shgetp_null(s->bundles, td->name);
    if(key_val)
    {
        *suc = true;
        bundle = key_val->value;
    }
    mtx_unlock(&(s->mtx));
    
    return bundle;
}

static int cmp_bundle(const void *v1, const void *v2)
{
    const jwtbundle_Bundle **b1 = (jwtbundle_Bundle**) v1, 
                            **b2 = (jwtbundle_Bundle**) v2;
    return strcmp((*b1)->td.name, (*b2)->td.name);
}

jwtbundle_Bundle** jwtbundle_Set_Bundles(jwtbundle_Set *s)
{
    mtx_lock(&(s->mtx));
    jwtbundle_Bundle **bundle_arr = NULL;

    for(size_t i = 0, size = shlenu(s->bundles); i < size; ++i)
    {
        arrput(bundle_arr, s->bundles[i].value);
    }

    qsort(bundle_arr, arrlenu(bundle_arr), sizeof(bundle_arr[0]), cmp_bundle);

    mtx_unlock(&(s->mtx));
    
    return bundle_arr;
}

uint32_t jwtbundle_Set_Len(jwtbundle_Set *s)
{
    mtx_lock(&(s->mtx));
    const uint32_t len = shlenu(s->bundles);
    mtx_unlock(&(s->mtx));
    
    return len;
}

jwtbundle_Bundle* jwtbundle_Set_GetJWTBundleForTrustDomain(
                                    jwtbundle_Set *s, 
                                    const spiffeid_TrustDomain *td, 
                                    err_t *err)
{
    mtx_lock(&(s->mtx));
    jwtbundle_Bundle *bundle = NULL;
    //trust domain not available
    *err = ERROR1;
    map_string_jwtbundle_Bundle *key_val = shgetp_null(s->bundles, td->name);
    if(key_val)
    {
        bundle = key_val->value;
        *err = NO_ERROR;
    }
    mtx_unlock(&(s->mtx));
    
    return bundle;
}