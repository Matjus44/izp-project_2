/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h> 
#include <stdbool.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

bool is_error = false;

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->obj = malloc(cap * sizeof(struct obj_t));
    if (c->obj == NULL)
    {
        exit(1);
    }
    c->capacity = cap;
    c->size = 1;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    if (c->capacity)
    free(c->obj);
    c->obj=NULL;
    c->size=0;
    c->capacity=0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if((c->capacity)==c->size)
    {
        c = resize_cluster(c,(c->capacity + CLUSTER_CHUNK));

        (c->obj[c->size]).id = obj.id;
        (c->obj[c->size]).x = obj.x;
        (c->obj[c->size]).y = obj.y;
        c->capacity = c->capacity + CLUSTER_CHUNK;
        c->size++;
    }
    else if(c->capacity > c->size)
    {
        (c->obj[c->size]).id = obj.id;
        (c->obj[c->size]).x = obj.x;
        (c->obj[c->size]).y = obj.y;
        c->size++;
    }
    
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for(int i=0;i < c2->size;i++)
    {
        struct obj_t obj;

        obj.id = c2->obj[i].id;
        obj.x = c2->obj[i].x;
        obj.y = c2->obj[i].y;
    
        append_cluster(c1,obj);
        sort_cluster(c1);

    }
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    struct cluster_t *removing_cluster = &(carr[idx]);
    clear_cluster(removing_cluster);
    int new_narr = narr - 1;
    for(; idx + 1 < narr; idx++)
    {
        carr[idx] = carr[idx + 1];
    }
    return new_narr;
    // TODO
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    double sub_x = (o1->x)-(o2->x);
    double sub_y = (o1->y)-(o2->y);
    float distance1 = (sub_x)*(sub_x)+(sub_y)*(sub_y);
    if(distance1<0)
    {
        distance1 = distance1 * (-1);
    }
    float obj_distance = sqrt(distance1);
    return obj_distance;
    // TODO
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    struct obj_t obj1;
    struct obj_t obj2;
    float min = INT_MAX;

    for(int i = 0 ;i < c1->size;i++)
    {
        obj1.x = c1->obj[i].x;
        obj1.y = c1->obj[i].y;

        for(int j = 0;j < c2->size;j++)
        {
            obj2.x = c2->obj[j].x;
            obj2.y = c2->obj[j].y;

            float vzdalenost = obj_distance(&obj1,&obj2);

            if(vzdalenost < min)
            {
                min = vzdalenost;
            }
        }  
    }
    return min;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float distance_C;
    float min_distance = INT_MAX;
    int index_c1 = 0;
    int index_c2 = 0;

    for(int j = 0; j < narr;j++)
    {
        for(int i = j+1; i < narr;i++)
        {
            distance_C = cluster_distance(&carr[j],&carr[i]);

            if(distance_C < min_distance)
            {
                min_distance = distance_C;
                index_c1 = j;
                index_c2 = i;
            }
        }
    }
    *c1 = index_c1;
    *c2 = index_c2;

  // TODO
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

int get_count(FILE*fpointer)
{
    char first_line[150]={0};
    fgets(first_line,150,fpointer);

    char first_line_arr[6]={'c','o','u','n','t','='};

    

    for(int i = 0; i < 6;i++)
    {
        if(first_line[i]!=first_line_arr[i])
        {
            is_error = true;
            int count = 0;
            return count;
        }
    }

    char amount_of_clusters[50]={0};

    for(size_t i=6;i<strlen(first_line);i++)
    {
        amount_of_clusters[i - 6]=first_line[i];  
    }

    int count = atoi(amount_of_clusters);

    for(size_t j = 0;j < (strlen(amount_of_clusters)-1);j++)
    {
        if(amount_of_clusters[j] >'9' || amount_of_clusters[j] < '0')
        {
            is_error = true;
            return count;
        }
    }

    return count;
}

bool is_char_valid(char c)
{
    if(c < '0' || c > '9')
    {
        return false;
    }
    return true;
}

float split(char* line, int* out_id, float* out_x, float* out_y)
{
    char id[100] = { 0 };
    char x[100] = { 0 };
    char y[100] = { 0 };
    
    size_t line_index = 0;
    size_t section_index = 0;
    
    // Get id
    while(line[line_index] != ' ')
    {
        id[section_index++] = line[line_index];

        if(is_char_valid(line[line_index]) == false)
        {
            return -1;
        }

        line_index++;
    }
    
    // Skip space
    line_index++;
    
    // Get x
    section_index = 0;
    while(line[line_index] != ' ')
    {
        x[section_index++] = line[line_index];

        if(is_char_valid(line[line_index]) == false)
        {
            return -1;
        }

        line_index++;
    }
    
    // Skip space
    line_index++;
    
    // Get y
    section_index = 0;
    while(line[line_index] != '\n' && line[line_index] != '\0')
    {
        y[section_index++] = line[line_index];

        if(is_char_valid(line[line_index]) == false)
        {
            return -1;
        }

        line_index++;
    }
    
    
    // Save results
    *out_id = atoi(id);
    *out_x = atoi(x);
    *out_y = atoi(y);
    return 0;
}


/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    FILE *fpointer;
    fpointer = fopen(filename,"r");
    if(fpointer==NULL)
    {
        return -1;
    }
    int count = get_count(fpointer);
    //printf("%d",count);

    char line[150];
    *arr = malloc(count*sizeof(struct cluster_t));

    for(int i=0;i<count;i++)
    {
        fgets(line,150,fpointer);
        int id = 0;
        float x = 0;
        float y = 0;
        
        split(line, &id, &x, &y);
        if(split(line, &id, &x, &y) == -1)
        {
            is_error = true;
            fclose(fpointer);
            return count;
        }

        if( x < 0 || x > 1000 || y < 0 || y >1000)
        {
            is_error = true;
            fclose(fpointer);
            return count;
        }

        init_cluster(&((*arr)[i]),CLUSTER_CHUNK);

        ((*arr)[i]).obj->id = id;
        ((*arr)[i]).obj->x = x;
        ((*arr)[i]).obj->y = y;
        ((*arr)[i]).size = 1;      
    }

    fclose(fpointer);
    return count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

bool is_argv2_valid(char *argument_2)
{

    for (size_t i = 0; i < strlen(argument_2); i++) 
    {
        if (argument_2[i] < '0' || argument_2[i] > '9')
        {
            return false;
        }
        if(atoi(argument_2) == 0)
        {
            return false;
        }
    }
    return true;
}

bool is_argv1_valid(char *argument_1)
{
    char array[7] = {'S','O','U','B','O','R'};

    if(strlen(argument_1) > strlen(array) || strlen(argument_1) < strlen(array))
    {
        return false;
    }

    for(int i = 0; i < 7; i++)
    {
        if(argument_1[i] != array[i])
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    if(argv == NULL || argc < 2 || argc > 3)
    {
        printf("invalid argument\n");
        return 1;
    }

    if(argc == 2)
    {
        if(is_argv1_valid(argv[1]) == false)
        {
            printf("wrong form of argv[1]\n");
            return 1;
        }

        int count = load_clusters(argv[1],&clusters);

        int index1 = 0;
        int index2 = 0;

        if(is_error == true)
        {
            for(int i = count-1; i >= 0; i--)
            {
                count = remove_cluster(clusters,count,i);
            }
            printf("invalid data\n");
            free(clusters);
            return -1;
        }
        
        while(count != 1)
        {
            find_neighbours(clusters,count,&index1,&index2);
            merge_clusters(&clusters[index1],&clusters[index2]);
            count = remove_cluster(clusters,count,index2);     
        }

        print_clusters(clusters,count);
        count = remove_cluster(clusters,count,0);
        free(clusters);
        return 0;
    }

    if(argc == 3)
    {
        char *input = argv[2];
        int input_num = atoi(argv[2]);

        if(is_argv1_valid(argv[1]) == false)
        {
            printf("wrong form of argv[1]\n");
            return 1;
        }

        //checking if input has valid characters.
        if (is_argv2_valid(input) == false) 
        {
            printf("invalid argument of argv[2]\n"); 
            return 1;
        }

        int count = load_clusters(argv[1],&clusters);

        int index1 = 0;
        int index2 = 0;

        if(is_error == true)
        {
            for(int i = count-1; i >= 0; i--)
            {
                count = remove_cluster(clusters,count,i);
            }
            free(clusters);
            printf("invalid data\n");
            return -1;
        }

        if(input_num > count)
        {
            for(int i = count-1; i >= 0; i--)
            {
                count = remove_cluster(clusters,count,i);
            }
            free(clusters);
            printf("invalid argument of argv[2]\n");
            return 1;
        }

        while(count > input_num)
        {
            find_neighbours(clusters,count,&index1,&index2);
            merge_clusters(&clusters[index1],&clusters[index2]);
            int new_count = remove_cluster(clusters,count,index2);
            count = new_count;
        }

        print_clusters(clusters,count);

        for(int i = count-1; i >= 0; i--)
        {
            count = remove_cluster(clusters,count,i);
        }

        free(clusters);
        return 0;
    }

return 0;

}

