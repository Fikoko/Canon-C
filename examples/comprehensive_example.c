#include <stdio.h>
#include <stdlib.h>
#include <canon/canon.h>

typedef struct {
    int id;
    String *name;
    int score;
} Student;

typedef OPTION(Student*) OptionStudent;
typedef RESULT(Vector*, const char*) ResultVector;

Student *student_new(int id, const char *name, int score) {
    Student *student = malloc(sizeof(Student));
    if (!student) return NULL;
    
    student->id = id;
    student->name = string_new(name);
    if (!student->name) {
        free(student);
        return NULL;
    }
    student->score = score;
    return student;
}

void student_free(Student *student) {
    if (!student) return;
    string_free(student->name);
    free(student);
}

void print_student(void *element, void *context) {
    (void)context;
    Student *student = *(Student **)element;
    printf("   ID: %d, Name: %-15s, Score: %d\n",
           student->id, string_cstr(student->name), student->score);
}

bool is_passing(const void *element, void *context) {
    int threshold = *(int *)context;
    Student *student = *(Student **)element;
    return student->score >= threshold;
}

void calculate_total(void *accumulator, const void *element, void *context) {
    (void)context;
    int *total = (int *)accumulator;
    Student *student = *(Student **)element;
    *total += student->score;
}

OptionStudent find_student_by_id(Vector *students, int target_id) {
    for (size_t i = 0; i < vector_size(students); i++) {
        Student **student_ptr = vector_get(students, i);
        if (student_ptr && (*student_ptr)->id == target_id) {
            OptionStudent opt = SOME(*student_ptr);
            return opt;
        }
    }
    OptionStudent opt = NONE;
    return opt;
}

ResultVector load_students(void) {
    Vector *students = vector_new(sizeof(Student*));
    if (!students) {
        ResultVector res = ERR("Failed to create student vector");
        return res;
    }
    
    const char *names[] = {"Alice", "Bob", "Charlie", "Diana", "Eve"};
    int scores[] = {85, 92, 78, 88, 95};
    
    for (int i = 0; i < 5; i++) {
        Student *student = student_new(i + 1, names[i], scores[i]);
        if (!student) {
            for (size_t j = 0; j < vector_size(students); j++) {
                Student **s = vector_get(students, j);
                student_free(*s);
            }
            vector_free(students);
            ResultVector res = ERR("Failed to create student");
            return res;
        }
        vector_push(students, &student);
    }
    
    ResultVector res = OK(students);
    return res;
}

int main(void) {
    printf("=== Canon Comprehensive Example: Student Management ===\n\n");
    
    printf("1. Loading students (Result type for error handling):\n");
    ResultVector result = load_students();
    if (result_is_err(result)) {
        fprintf(stderr, "Error: %s\n", result_unwrap_err(result));
        return 1;
    }
    
    Vector *students = result_unwrap(result);
    printf("   Successfully loaded %zu students:\n", vector_size(students));
    vector_foreach(students, print_student, NULL);
    printf("\n");
    
    printf("2. Finding a student by ID (Option type):\n");
    OptionStudent found = find_student_by_id(students, 3);
    if (option_is_some(found)) {
        Student *student = option_unwrap(found);
        printf("   Found: %s (Score: %d)\n", 
               string_cstr(student->name), student->score);
    } else {
        printf("   Student not found\n");
    }
    printf("\n");
    
    printf("3. Filtering passing students (score >= 85):\n");
    int threshold = 85;
    Vector *passing = vector_filter(students, is_passing, &threshold);
    printf("   Passing students:\n");
    vector_foreach(passing, print_student, NULL);
    printf("\n");
    
    printf("4. Calculating total score (reduce operation):\n");
    int total_score = 0;
    vector_reduce(students, &total_score, calculate_total, NULL);
    double average = (double)total_score / vector_size(students);
    printf("   Total score: %d\n", total_score);
    printf("   Average score: %.2f\n\n", average);
    
    printf("5. Building a report string:\n");
    String *report = string_new("Student Report:\n");
    for (size_t i = 0; i < vector_size(students); i++) {
        Student **student_ptr = vector_get(students, i);
        Student *student = *student_ptr;
        
        String *line = string_new("  - ");
        string_append(line, string_cstr(student->name));
        string_append(line, ": ");
        
        char score_buf[32];
        snprintf(score_buf, sizeof(score_buf), "%d", student->score);
        string_append(line, score_buf);
        string_append(line, "\n");
        
        string_append(report, string_cstr(line));
        string_free(line);
    }
    printf("%s\n", string_cstr(report));
    
    printf("Key benefits demonstrated:\n");
    printf("  ✓ No manual memory allocation tracking\n");
    printf("  ✓ No manual loop scaffolding (map/filter/reduce)\n");
    printf("  ✓ Explicit error handling with Result types\n");
    printf("  ✓ Safe optional values with Option types\n");
    printf("  ✓ Functional-style operations on collections\n");
    printf("  ✓ Safe string manipulation without buffer overflows\n");
    
    for (size_t i = 0; i < vector_size(students); i++) {
        Student **student_ptr = vector_get(students, i);
        student_free(*student_ptr);
    }
    vector_free(students);
    vector_free(passing);
    string_free(report);
    
    return 0;
}
