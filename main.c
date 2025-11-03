#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HABITS 5
#define MAX_NAME 40
#define FILENAME "habits.dat"

typedef struct {
    char name[MAX_NAME];
    int target;
    int days[7];
    int weeks_tracked;
    int weeks_met_target;
} Habit;

void load_data(Habit habits[], int *count) {
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) { *count = 0; return; }
    if (fread(count, sizeof(int), 1, fp) != 1) { *count = 0; fclose(fp); return; }
    if (*count > MAX_HABITS) *count = MAX_HABITS;
    fread(habits, sizeof(Habit), *count, fp);
    fclose(fp);
}

void save_data(Habit habits[], int count) {
    FILE *fp = fopen(FILENAME, "wb");
    if (!fp) { perror("Save error"); return; }
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(habits, sizeof(Habit), count, fp);
    fclose(fp);
}

void print_header(void) {
    printf("------- Smart Habit Tracker -------\n");
}

void print_menu(void) {
    print_header();
    printf(" 1  Add New Habit\n");
    printf(" 2  Mark Today's Progress\n");
    printf(" 3  Weekly Report \n");
    printf(" 4  Reset Week \n");
    printf(" 5  Save & Exit \n");
    printf("\n");
    printf("Enter choice: ");
}

void list_habits(Habit habits[], int count) {
    if (count == 0) { printf("No habits added yet.\n"); return; }
    printf("\nIndex  Habit (target/day)\n");
    printf("--------------------------\n");
    for (int i = 0; i < count; ++i) {
        printf("%d) %s (%d)\n", i + 1, habits[i].name, habits[i].target);
    }
}

void add_habit(Habit habits[], int *count) {
    if (*count >= MAX_HABITS) {
        printf("Maximum habits reached (%d). Delete or reset to add more.\n", MAX_HABITS);
        return;
    }
    Habit h;
    getchar();
    printf("Enter habit name: ");
    fgets(h.name, MAX_NAME, stdin);
    h.name[strcspn(h.name, "\n")] = '\0';
    if (strlen(h.name) == 0) { printf("Name cannot be empty.\n"); return; }

    printf("Enter weekly target (0-7 days): ");
    if (scanf("%d", &h.target) != 1) { printf("Invalid input.\n"); return; }
    if (h.target < 0) h.target = 0;
    if (h.target > 7) h.target = 7;

    for (int i = 0; i < 7; ++i) h.days[i] = 0;
    h.weeks_tracked = 0;
    h.weeks_met_target = 0;

    habits[*count] = h;
    (*count)++;
    printf("Habit added: %s (target %d days/week)\n", h.name, h.target);
}

void mark_progress(Habit habits[], int count) {
    if (count == 0) { printf("No habits to mark. Add one first.\n"); return; }
    list_habits(habits, count);
    int idx;
    printf("Choose habit index: ");
    if (scanf("%d", &idx) != 1) { printf("Invalid input.\n"); return; }
    if (idx < 1 || idx > count) { printf("Invalid index.\n"); return; }

    Habit *h = &habits[idx - 1];
    int day;
    printf("Enter day number (1-Mon ... 7-Sun): ");
    if (scanf("%d", &day) != 1) { printf("Invalid input.\n"); return; }
    if (day < 1 || day > 7) { printf("Invalid day.\n"); return; }

    int val;
    printf("Mark completed? (1 = Yes, 0 = No): ");
    if (scanf("%d", &val) != 1) { printf("Invalid input.\n"); return; }

    h->days[day - 1] = (val != 0) ? 1 : 0;
    printf("Marked '%s' for day %d as %s.\n", h->name, day, h->days[day - 1] ? "Done" : "Not Done");
}

void show_report(Habit habits[], int count) {
    if (count == 0) { printf("No habits to report.\n"); return; }
    printf("\nWeekly Progress Report:\n");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < count; ++i) {
        Habit *h = &habits[i];
        int completed = 0;
        for (int d = 0; d < 7; ++d) completed += h->days[d];
        float percent = (completed / 7.0f) * 100.0f;
        int stars = (int)(percent / 20.0f + 0.5f);
        if (stars > 5) stars = 5;

        printf("Habit: %s\n", h->name);
        printf("Target: %d days/week | Completed: %d days | Progress: %.1f%%\n",
               h->target, completed, percent);
        printf("Badge: ");
        if (stars == 0) printf("*");
        for (int s = 0; s < stars; ++s) printf("*");
        printf("\n");
        if (completed >= h->target) printf("Status: Target met this week!\n");
        else printf("Status: Keep going - target not reached.\n");
        printf("Weeks tracked: %d | Weeks met target: %d\n", h->weeks_tracked, h->weeks_met_target);
        printf("-----------------------------------------------------\n");
    }
}

void reset_week(Habit habits[], int count) {
    if (count == 0) { printf("No habits to reset.\n"); return; }
    for (int i = 0; i < count; ++i) {
        Habit *h = &habits[i];
        int completed = 0;
        for (int d = 0; d < 7; ++d) completed += h->days[d];
        if (completed >= h->target) h->weeks_met_target++;
        h->weeks_tracked++;
        for (int d = 0; d < 7; ++d) h->days[d] = 0;
    }
    printf("Week reset complete. Previous week archived into stats.\n");
}

int main(void) {
    Habit habits[MAX_HABITS];
    int count = 0;
    load_data(habits, &count);

    while (1) {
        print_menu();
        int choice;
        if (scanf("%d", &choice) != 1) { printf("Invalid input.\n"); break; }
        switch (choice) {
            case 1: add_habit(habits, &count); break;
            case 2: mark_progress(habits, count); break;
            case 3: show_report(habits, count); break;
            case 4: reset_week(habits, count); break;
            case 5: save_data(habits, count); printf("Saved. Goodbye!\n"); return 0;
            default: printf("Invalid choice. Try again.\n"); break;
        }
    }
    return 0;
}