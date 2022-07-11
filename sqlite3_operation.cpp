#include "sqlite3_operation.h"
sqlite3 *db = NULL; //声明sqlite关键结构指针
char *zErrMsg = 0;
int rc, id;
sqlite3_stmt *stmt;

int Connection_Sqlite3DataBase()
{
    rc = sqlite3_open("./face_database/face.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    else
        printf("You have opened a sqlite3 database named bind.db successfully!\nCongratulation! Have fun!\n");

    return 0;
}

void insert_face_data_toDataBase(const char *name, MByte *face_feature, MInt32 featureSize)
{
    sqlite3_prepare(db, "insert into face_data_table(name,face_feature,feature_size) values (?,?,?);", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name, strlen(name), NULL);
    sqlite3_bind_blob(stmt, 2, face_feature, featureSize, NULL);
    sqlite3_bind_int(stmt, 3, featureSize);
    sqlite3_step(stmt);
}

map<string, ASF_FaceFeature> QueryFaceFeature()
{
    ASF_FaceFeature asf_feature = {0, 0};
    map<string, ASF_FaceFeature> map;
    sqlite3_stmt *stmt;
    char *sql = "select name, feature_size, face_feature from face_data_table";
    int ret = sqlite3_prepare(db, sql, strlen(sql), &stmt, 0);
    int id = 0, len = 0;
    char * name;
    int feature_size;

    if (ret == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            name = (char *)sqlite3_column_text(stmt, 0);
            printf("name = %s\n", name);
            feature_size = sqlite3_column_int(stmt, 1);
            printf("feature_size = %d\n", feature_size);
            asf_feature.feature = (MByte *)malloc(feature_size);
            const void *feature = sqlite3_column_blob(stmt, 2);   
            memset(asf_feature.feature, 0, feature_size);
            memcpy(asf_feature.feature, feature, feature_size);
            asf_feature.featureSize = feature_size;
            string str(name);
            map.insert(pair<string, ASF_FaceFeature>(str, asf_feature));
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db); 

    return map;
}

#if 0
int main()
{
    const char * name = "KKKK";
    int feature_size = 20;
    Connection_Sqlite3DataBase();
    //insert_face_data_toDataBase(name, feature_size);
    return 0;
}
#endif