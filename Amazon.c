#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_PRODUCTS 10

// Estruturas para os registros
typedef struct {
    int id;
    char name[MAX_NAME];
    float price;
    int quantity;
} Product;

typedef struct {
    int id;
    char name[MAX_NAME];
    int product_ids[MAX_PRODUCTS];
    int product_count;
} Customer;

// Funções auxiliares
void pause() {
    printf("Pressione Enter para continuar...\n");
    getchar();
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Funções de manipulação de arquivos
void add_product(const char *filename) {
    Product p;
    FILE *file = fopen(filename, "ab");

    if (!file) {
        perror("Erro ao abrir o arquivo de produtos");
        return;
    }

    printf("ID do produto: ");
    scanf("%d", &p.id);
    clear_stdin();
    printf("Nome do produto: ");
    fgets(p.name, MAX_NAME, stdin);
    p.name[strcspn(p.name, "\n")] = '\0';
    printf("Preco do produto: ");
    scanf("%f", &p.price);
    printf("Quantidade em estoque: ");
    scanf("%d", &p.quantity);

    fwrite(&p, sizeof(Product), 1, file);
    fclose(file);
    printf("Produto adicionado com sucesso!\n");
}

void add_customer(const char *filename) {
    Customer c;
    FILE *file = fopen(filename, "ab");

    if (!file) {
        perror("Erro ao abrir o arquivo de clientes");
        return;
    }

    printf("ID do cliente: ");
    scanf("%d", &c.id);
    clear_stdin();
    printf("Nome do cliente: ");
    fgets(c.name, MAX_NAME, stdin);
    c.name[strcspn(c.name, "\n")] = '\0';

    c.product_count = 0;
    printf("Quantos produtos este cliente comprou? ");
    scanf("%d", &c.product_count);

    if (c.product_count > MAX_PRODUCTS) {
        printf("Numero de produtos excede o limite (%d).\n", MAX_PRODUCTS);
        c.product_count = MAX_PRODUCTS;
    }

    for (int i = 0; i < c.product_count; i++) {
        printf("ID do produto %d: ", i + 1);
        scanf("%d", &c.product_ids[i]);
    }

    fwrite(&c, sizeof(Customer), 1, file);
    fclose(file);
    printf("Cliente adicionado com sucesso!\n");
}

void list_products(const char *filename) {
    Product p;
    FILE *file = fopen(filename, "rb");

    if (!file) {
        perror("Erro ao abrir o arquivo de produtos");
        return;
    }

    printf("\nProdutos cadastrados:\n");
    while (fread(&p, sizeof(Product), 1, file)) {
        printf("ID: %d\nNome: %s\nPreco: %.2f\nQuantidade: %d\n\n", p.id, p.name, p.price, p.quantity);
    }

    fclose(file);
}

void list_customers(const char *filename, const char *product_file) {
    Customer c;
    Product p;
    FILE *file = fopen(filename, "rb");
    FILE *prod_file = fopen(product_file, "rb");

    if (!file || !prod_file) {
        perror("Erro ao abrir os arquivos");
        return;
    }

    printf("\nClientes cadastrados:\n");
    while (fread(&c, sizeof(Customer), 1, file)) {
        printf("ID: %d\nNome: %s\nProdutos comprados:\n", c.id, c.name);

        for (int i = 0; i < c.product_count; i++) {
            rewind(prod_file);
            while (fread(&p, sizeof(Product), 1, prod_file)) {
                if (p.id == c.product_ids[i]) {
                    printf("  - %s\n", p.name);
                    break;
                }
            }
        }
        printf("\n");
    }

    fclose(file);
    fclose(prod_file);
}

void remove_product(const char *filename) {
    int id;
    Product p;
    FILE *file = fopen(filename, "rb");
    FILE *temp = fopen("temp.bin", "wb");

    if (!file || !temp) {
        perror("Erro ao abrir os arquivos");
        return;
    }

    printf("ID do produto a ser removido: ");
    scanf("%d", &id);

    while (fread(&p, sizeof(Product), 1, file)) {
        if (p.id != id) {
            fwrite(&p, sizeof(Product), 1, temp);
        }
    }

    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.bin", filename);

    printf("Produto removido com sucesso!\n");
}

// Menu principal
int main() {
    const char *product_file = "products.bin";
    const char *customer_file = "customers.bin";
    int choice;

    do {
        printf("\nMenu:\n");
        printf("1. Adicionar produto\n");
        printf("2. Adicionar cliente\n");
        printf("3. Listar produtos\n");
        printf("4. Listar clientes\n");
        printf("5. Remover produto\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_product(product_file);
                break;
            case 2:
                add_customer(customer_file);
                break;
            case 3:
                list_products(product_file);
                break;
            case 4:
                list_customers(customer_file, product_file);
                break;
            case 5:
                remove_product(product_file);
                break;
            case 0:
                printf("Encerrando o programa.\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }

        pause();
    } while (choice != 0);

    return 0;
}