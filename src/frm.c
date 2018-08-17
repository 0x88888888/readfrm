#include "defs.h"

uchar frm_header_buff[FRM_HEAD_LEN];
uchar frm_forminfo_buff[FRM_FORMINFO_LEN];

uchar const* path;
FILE* frm;

uchar frm_hdr_legacy_db_type;
uint32 key_info_length;
uint32 keybuff_file_pos;
uint32 key_count;
uint32 reclength;
uint32 key_buff_length;
uchar partition_default_engine_type =0;
uint create_fields__elements =0;
uint null_fields =0;
uint fixed_field_pack_length = 17 ; //低版本的frm文件，本程序不支持
uint forminfo_pos;
uint screen_buff_length;
uint n_length;
uint int_length;
uint int_count;
uint com_length;

void patch() {
    legacy_db_types[42] ="DB_TYPE_FIRST_DYNAMIC";
    legacy_db_types[127] = "DB_TYPE_DEFAULT";
} 


void show_header(FILE* frm, uchar frm_header_buff[FRM_HEAD_LEN] ) {
    printf("%08d  magic00: 0x%02x \n", 0, frm_header_buff[0]);
    printf("%08d  magic01: 0x%02x \n", 1, frm_header_buff[1]);
    printf("%08d  FRM_VER: 0x%02x \n", 2, frm_header_buff[2]);
    frm_hdr_legacy_db_type = frm_header_buff[3];
    printf("%08d  legacy_db_type index: 0x%02x \n", 3, frm_hdr_legacy_db_type);
    printf("%08d  (n_length + length) : 0x%02x  make_new_entry 中确定 newname的长度，从文件的64bit出开始读\n", 4, frm_header_buff[4]);
    printf("%08d  unknown: 0x%02x \n", 5, frm_header_buff[5]);
    keybuff_file_pos = uint2korr(frm_header_buff + 6);
    printf("%08d  IO_SIZE(keybuff_file_pos): 0x%04x \n", 6, keybuff_file_pos);
    printf("%08d  unknown(names): 0x%04x    get_form_pos 中 names\n", 8, uint2korr(frm_header_buff + 8));
    
    printf("%08d  length : 0x%08x -->%d \n", 10, uint4korr(frm_header_buff + 10), uint4korr(frm_header_buff + 10));
    printf("            key_length= keys * (8 + MAX_REF_PARTS * 9 + NAME_LEN + 1) + 16 + key_comment_total_bytes\n");
    printf("            length= next_io_size((ulong) (IO_SIZE + key_length+reclength + create_info->extra_size))\n");
    printf("\n");

    printf("%08d  tmp_key_length: 0x%04x\n", 14 ,uint2korr(frm_header_buff + 14));

    printf("%08d  reclength: 0x%04x \n", 16, uint2korr(frm_header_buff + 16));
    printf("%08d  create_info->max_rows: 0x%04x \n", 18, uint4korr(frm_header_buff + 18));
    printf("%08d  create_info->min_rows: 0x%04x \n", 22, uint4korr(frm_header_buff + 22));
    printf("%08d  unknown: 0x%02x    (create_info->max_rows == 1) &&(create_info->min_rows == 1) && (keys == 0)在mysql_create_frm这样计算出来的 \n", 26, frm_header_buff[26]);
    printf("%08d  new_field_pack_flag: 0x%02x \n", 27, frm_header_buff[27]);
    key_info_length=uint2korr(frm_header_buff + 28);
    printf("%08d  key_info_length: 0x%04x \n", 28, key_info_length);
    printf("%08d  create_info->table_options: 0x%04x \n", 30, uint2korr(frm_header_buff + 30));
    printf("%08d  No filename anymore: 0x%02x \n", 32, frm_header_buff[32]);
    printf("%08d  Mark for 5.0 frm file: 0x%02x \n", 33, frm_header_buff[33]);
    printf("%08d  create_info->avg_row_length: 0x%04x \n", 34, uint4korr(frm_header_buff + 34));
    printf("%08d  csid(charset id): 0x%02x \n", 38, frm_header_buff[38]);
    printf("%08d  for future versions: 0x%02x \n", 39, frm_header_buff[39]);
    printf("%08d  create_info->row_type: 0x%02x \n", 40, frm_header_buff[40]);
    printf("            Bytes 41-46 were for RAID support; now reused for other purposes\n");
    printf("%08d  (csid >> 8): 0x%02x \n", 41, frm_header_buff[41]);
    printf("%08d  create_info->stats_sample_pages: 0x%04x \n", 42, uint2korr(frm_header_buff + 42));
    printf("%08d  create_info->stats_auto_recalc: 0x%02x \n", 44, frm_header_buff[44]);
    printf("%08d  unknown: 0x%02x \n", 45, frm_header_buff[45]);
    printf("%08d  unknown: 0x%02x \n", 46, frm_header_buff[46]);
    key_buff_length = uint4korr(frm_header_buff + 47);
    printf("%08d  key_length (key_buff_length): 0x%08x --> %d\n", 47, key_buff_length , key_buff_length);
    printf("%08d  MYSQL_VERSION_ID: 0x%08x \n", 51, uint4korr(frm_header_buff + 51));
    printf("%08d  create_info->extra_size: 0x%08x \n", 55, uint4korr(frm_header_buff + 55));
    printf("%08d  59-60 is reserved for extra_rec_buf_length: 0x%04x \n", 59, uint2korr(frm_header_buff + 59));
    partition_default_engine_type = frm_header_buff[61];
    printf("%08d  partition default_part_db_type: 0x%02x \n", 61, partition_default_engine_type);
    printf("%08d  create_info->key_block_size: 0x%04x \n", 62, uint2korr(frm_header_buff + 26));
}

int show_keybuff(FILE *frm, uint32 keybuff_pos_of_file, uint32 keybuff_len) {
	int err;
    err=fseek(frm,keybuff_pos_of_file, SEEK_SET);

    uchar* keybuff= malloc(keybuff_len);
    if(keybuff == NULL) {
    	printf("show_keybuff malloc error\n");
    	exit(0);
    	return 0;
    }
    fread( keybuff, keybuff_len, 1, frm);

    if(keybuff[2]==0 && keybuff[3]==0){
    	//key_count和key_part都小于127
    	key_count= keybuff[0];
        printf("%08d  key_count=%d\n", 0, key_count);    	
        printf("%08d  key_parts=%d\n", 1, keybuff[1]);
    } else {
    	printf("%08d  没有读取key_count和key_parts信息\n",0);
    }
    printf("%08d  pos-keyname_pos=0x%x\n",4, uint2korr(keybuff + 4));

    int key_idx =0;
    uint key_parts =0;
    uint pos=0+6;
    for(key_idx = 0; key_idx < key_count ; key_idx++) {
    	printf("keyidx = %d\n", key_idx);

    	printf("%08d  key->flags=0x%04x\n",pos ,uint2korr(keybuff + pos) );
    
    	printf("%08d  key->key_length=0x%04x\n",pos+2 ,uint2korr(keybuff + pos + 2) );

    	printf("%08d  key->user_defined_key_parts (key_parts)=0x%04x\n",pos+4 ,keybuff[pos+4] );

    	printf("%08d  key->algorithm=0x%04x\n",pos+5 ,keybuff[pos+5] );

        printf("%08d  key->block_size=0x%04x\n",pos +6,uint2korr(keybuff + pos +6) );
        
        key_parts = keybuff[pos+4];
        pos +=8;
        for(int i=0; i<key_parts; i++) {
        	printf("\t%08d  key_part->fieldnr+1+FIELD_NAME_USED = 0x%04x\n", pos , uint2korr(keybuff + pos) );
        	printf("\t%08d  key_part->offset+data_offset+1 = 0x%04x\n", pos +2 , uint2korr(keybuff + pos +2 ) );
        	printf("\t%08d  Sort order=%d\n", pos+4 , keybuff[pos+4]);
        	printf("\t%08d  key_part->key_type = 0x%04x\n", pos +5 , uint2korr(keybuff + pos +5 ) );
        	printf("\t%08d  key_part->length = 0x%04x\n", pos +7 , uint2korr(keybuff + pos +7 ) );
        	printf("------------------------\n");
        	pos+=9;
        }

    }

    //key names
    uchar* keyname_pos = keybuff + pos;
    uint key_name_len=0;
    
    printf("%08d   NAMES_SEP_CHAR=0x%04x \n", pos, keybuff[pos]);
    pos++;
    key_name_len = strlen( (keybuff+ pos));
    uchar key_name[256] = {0};
    int idx1=0,idx2=0;
    for(int i=0; i<key_count; i++) {
        memset(key_name, 0,256);
        idx2=0;
        while(idx1 < key_name_len) {
            if( keyname_pos[idx1 + 1] == 0xff ) {
            	idx1++;
            	break;
            }
            key_name[idx2++] = keyname_pos[idx1 + 1];
            idx1++;
        }

        printf("%08d  keyname%d = %s  \n",pos + idx1 - 1 - strlen(key_name), i, key_name);
    }

    pos = pos + key_name_len + 1;
    //key comment information
    for(int i=0; i<key_count; i++) { 
    	 memset(key_name, 0, 256);
         int comment_length= uint2korr(keybuff+pos);
         if(comment_length == 0) {
         	break;
         }
         printf("%08d  key->comment.length =%d\n", pos, comment_length);
         pos+=2;
         strncpy(key_name, keybuff + pos,comment_length);
         printf("%08d  key->comment.str = %s\n", comment_length, key_name);
         pos+=comment_length;
    }

    printf("keybuff length =%d\n",pos);
    free(keybuff);
    return pos;
}

void show_default_info(FILE *frm,  uint32 keybuff_pos_of_file, uint32 keybuff_len) {
    int err =0;
    err=fseek(frm, keybuff_pos_of_file + keybuff_len, SEEK_SET);
}


int get_forminfo_pos(FILE* frm) {
	uint fixed_fileinfo4_n_length = 1;
	uint fixed_fileinfo8_names = 0;
	uint fixed_length = 2;
	int err;
	err=fseek(frm, 63 + fixed_fileinfo4_n_length + fixed_length + fixed_fileinfo8_names*4 + 1, SEEK_SET);
	int forminfo_pos=0;
	fread(&forminfo_pos, 4, 1 , frm);
	

    return forminfo_pos;
}

void show_forminfo(FILE* frm) {
	int err;
	uchar forminfo[288];
	forminfo_pos = get_forminfo_pos(frm);
	printf("forminfo_pos = 0x%x\n",forminfo_pos);
    err = fseek(frm, forminfo_pos, SEEK_SET);
    fread(forminfo, 288, 1, frm);
    /*
    for(int i=0; i<288; i++) {
    	if(i %16 ==0){
    		printf("\n");
    	}
    	printf("%02x ", forminfo[i]);
    }
    printf("\n%02x  %02x %02x %02x \n",forminfo[0],forminfo[1],forminfo[2],forminfo[3]);
    */
    printf("%08d  maxlength=0x%04x \n", 2, uint2korr(forminfo + 2));

    printf("%08d  create_info->comment.length=0x%04x \n", 46, forminfo[46]);
    if(forminfo[46]) {
    	printf("%08d  create_info->comment.str= %s \n", 47, (forminfo+47));
    }
    

    printf("%08d  screens=0x%04x \n", 256, forminfo[256]);
    create_fields__elements = uint2korr(forminfo + 258);
    printf("%08d  create_fields.elements=0x%04x \n", 258, create_fields__elements);
    screen_buff_length =  uint2korr(forminfo + 260);
    printf("%08d  info_length=0x%04x (Length of all screens)\n", 260, screen_buff_length);
    printf("%08d  totlength=0x%04x \n", 262, uint2korr(forminfo + 262));
    printf("%08d  no_empty=0x%04x \n", 264, uint2korr(forminfo + 264));
    reclength= uint2korr(forminfo + 266);
    printf("%08d  reclength=0x%04x \n", 266, reclength);
    n_length = uint2korr(forminfo + 268);
    printf("%08d  n_length=0x%04x \n", 268, n_length);
    int_count = uint2korr(forminfo + 270);
    printf("%08d  int_count (interval_count)=0x%04x \n", 270, int_count);
    printf("%08d  int_parts (interval_parts)=0x%04x \n", 272, uint2korr(forminfo + 272));
    int_length = uint2korr(forminfo + 274);
    printf("%08d  int_length (int_length)=0x%04x \n", 274, int_length);
    printf("%08d  time_stamp_pos=0x%04x \n", 276, uint2korr(forminfo + 276));
    printf("%08d  screen columns=0x%04x \n", 278, 80);
    printf("%08d  screen rows=0x%04x \n", 280, 22);
    null_fields = uint2korr(forminfo + 282);
    printf("%08d  null_fields=0x%04x \n", 282, null_fields);
    com_length = uint2korr(forminfo + 284);
    printf("%08d  com_length=0x%04x \n", 284, com_length);

}

void show_defaults(FILE* frm) {

}

#define BUF_SIZ    1024
uchar* buff;
uchar* tmp;

void show_misc(FILE* frm, uint keybuff_file_pos, uint keybuff_len, uint reclength) {
    int err;
     buff = malloc(BUF_SIZ);
     tmp = malloc(BUF_SIZ);
     memset(buff, 0, BUF_SIZ);
     memset(buff, 0, BUF_SIZ);
    
    
    uint pos = keybuff_file_pos + keybuff_len + reclength;
    printf("xpos=%08x keybuff_len = %04x reclength=%04x\n", pos, keybuff_len, reclength);
    err = fseek(frm, pos , SEEK_SET);
    uint pos2 = keybuff_len + reclength;
    uint cur_pos=0;

    fread(buff, BUF_SIZ, 1, frm);
    //connect
    uint connect_len = uint2korr(buff);
    
    printf("%08x  create_info->connect_string.length=%d\n", pos, connect_len);
    cur_pos +=2;
    if (connect_len) {
    	memset(tmp, 0, BUF_SIZ);
    	strncpy(tmp, buff+cur_pos, connect_len);
    	printf("%08x  create_info->connect_string.str=%s\n", pos +cur_pos , tmp);
    	cur_pos += connect_len;
    }
    
    uint str_db_type_len = uint2korr(buff+cur_pos);
    printf("%08x  str_db_type.length=%d\n", pos +cur_pos, str_db_type_len);
    cur_pos += 2;
    if(str_db_type_len) {
    	memset(tmp, 0, BUF_SIZ);
    	strncpy(tmp, buff+cur_pos, str_db_type_len);
    	//printf("str_db_type_len =%d \n", str_db_type_len);
    	printf("%08x  str_db_type.str=%s\n", pos +cur_pos, tmp);
    	cur_pos += str_db_type_len;
    }
    
    if (partition_default_engine_type) {
    	uint partition_info_len=uint4korr(buff + cur_pos);
    	printf("%08x  part_info->part_info_len=%08x \n", pos + cur_pos, partition_info_len);
    	cur_pos += 4;
    	cur_pos +=1;
        memset(tmp, 0, BUF_SIZ);
        strncpy(tmp, buff+cur_pos, partition_info_len);
        printf("%08x  part_info->part_info_string=%s \n", pos+ cur_pos, tmp);
        cur_pos += partition_info_len;
        //cur_pos += 1;

        uchar auto_partitioned =0;
        auto_partitioned = buff[cur_pos];
        printf("%08x auto_partitioned=%d\n", pos+cur_pos, auto_partitioned);

    } else {
    	cur_pos +=6;
    }

    //parser_name跳过去，可能会出错

    const uint fixed_format_section_header_size= 8;
    const uint fixed_tablespace_length = 0;
    
    uint format_section_length;
    /*
    uint format_section_length= fixed_format_section_header_size + 
	                     fixed_tablespace_length + 1 +
                         create_fields__elements;
    */                     
    cur_pos++;

    format_section_length= uint2korr(buff + cur_pos);
    printf("%08x  format_section_length = %x\n", pos+ cur_pos, format_section_length);

    cur_pos += 2;
    uint format_section_flags = uint4korr(buff + cur_pos);
    printf("%08x  format_section_flags = %x \n", pos+ cur_pos, format_section_flags);
    cur_pos += 4;
    uint format_section_unused = uint2korr(buff + cur_pos);
    printf("%08x  format_section_unused = %x \n", pos+ cur_pos, format_section_unused);
    cur_pos += 2;
  
    cur_pos += fixed_tablespace_length;
    cur_pos += 1;

    for(int i=0; i<create_fields__elements; i++){
    	printf("%08x  (%4d) field_flags  = %x \n", pos+ cur_pos, i, buff[pos+cur_pos]);
    	cur_pos +=1;
    }


    free(tmp);
    free(buff);
}

void show_fields_info(FILE* frm, 
	                  uint formpos,
                      uint fields, 
                      uint field_pack_length, 
                      uint screen_buff_length /* uint2korr(forminfo+260) */ ,
                      uint n_length /*uint2korr(forminfo+268)*/,
                      uint int_length /*uint2korr(forminfo+274)*/,
                      uint com_length /*uint2korr(forminfo+284) */) {
    

    printf("formpos=0x%x\n",formpos);
    printf("fields=0x%x\n",fields);
    printf("field_pack_length=0x%x\n",field_pack_length);
    printf("screen_buff_length=0x%x\n",screen_buff_length);
    printf("n_length=0x%x\n",n_length);
    printf("int_length=0x%x\n",int_length);
    printf("com_length=0x%x\n",com_length);

    //跳过forminfo
    fseek(frm, formpos + 288, SEEK_SET);
    uchar* strpos = 0;

    uint read_length = fields*field_pack_length + screen_buff_length + n_length + int_length + com_length;
    uchar* buff = malloc(read_length + 12);;
    memset(buff, 0, read_length +12);
    fread(buff, read_length, 1, frm);
    
    /*
    for(int i=0; i<read_length; i++) {
    	if(i>0 && i%16 ==0) {
             printf("\n");
    	}
    	printf("%02x ",buff[i]);
    }
    printf("\n");
    printf("\n\n");
    */
    //跳过screen_buffer
    strpos = buff + screen_buff_length;
    
    //各种名称长度，包括field name 
    uint fields_name_length = n_length ; 
    uchar* fields_name_buff = malloc(fields_name_length);
    memcpy(fields_name_buff , strpos + create_fields__elements * field_pack_length, fields_name_length);
    /*
    printf("%s\n", fields_name_buff);
    for(int i=0; i< fields_name_length; i++) {
    	if(i>0 && i%16 ==0) {
             printf("\n");
    	}
    	printf("%02x ",fields_name_buff[i]);
    }
    printf("\n");
    */
    /*
    printf("*************************************\n");
    for(int i=0; i<create_fields__elements; i++){
    	printf("\n");
    	for(int j=0; j<fixed_field_pack_length; j++){
    	    printf("%02x ",strpos[i * fixed_field_pack_length +j]);	
    	}
    }
    printf("\n*************************************\n");
    */
    char** fields_name_arr;
    uint fields_name_arr_len=0;
    fields_name_arr=split_string(fields_name_buff, "\377", &fields_name_arr_len);
    printf("fields_name_arr_len=%d\n", fields_name_arr_len);
    uchar* tmppos=strpos;
    for(int i=0; i<fields_name_arr_len; i++) {
    	printf("field name: %s \n", fields_name_arr[i]);
    	
    	//uchar* flags_start = strpos+ i * fixed_field_pack_length;
        tmppos +=    fixed_field_pack_length;
        /*
        for(int k=0; k < fixed_field_pack_length; k++) {
          printf("%02x ", strpos[k]);
        }
        printf("\n");
        */ 
        printf("\tbyte0= %02x byte1=%02x byte2=%02x \n", tmppos[0],tmppos[1],tmppos[2]);
        printf("\tfield_length:0x%04x ",uint2korr(tmppos+3));
        printf("\trecpos:0x%04x \n",uint3korr(tmppos+5));
        printf("\tpack_flag:0x%04x ",uint2korr(strpos+8));
        printf("\tunireg_type:0x%04x \n",tmppos[10]);
        printf("\tpos_11:0x%04x ",tmppos[11]);
        printf("\tinterval_nr:0x%04x \n",tmppos[12]);
        printf("\tfield_type:0x%04x ",tmppos[13]);
        printf("\tgeom_type:0x%04x \n",tmppos[14]);
        printf("\tcomment_length:0x%04x ",uint2korr(tmppos+15));
    

        printf("\n");

    }
    
    printf("\n");
    uchar* interval_elements_buff;
    interval_elements_buff = malloc(int_length);
    memcpy(interval_elements_buff, 
    	   strpos + create_fields__elements * field_pack_length + n_length, int_length );
    
    int cur_len=0;
    int total_len=0;
    uchar** intervals_arr=NULL;
    uint intervals_arr_len =0;
    /*
    for(int i=0; i <int_length; i++) {
    	if(i>0 && i%16 ==0) {
            printf("\n");
    	}
    	printf("%02x ",interval_elements_buff[i]);
    }
    */
    printf("intervals:\n");
    int x=0;
    while(total_len< int_length) {
    	cur_len = strlen(interval_elements_buff+total_len);
    	printf("enum or set %d:\n", x++);
    	intervals_arr=split_string(interval_elements_buff + total_len, "\377", &intervals_arr_len);
    	for(int i=0; i <intervals_arr_len; i++) {
             printf("\tinterval: %s\n" , intervals_arr[i]);
    	}
    	//printf("\n");
    	total_len +=cur_len;
    	total_len += 1;
        //printf("total_len = %d  cur_len=%d\n", total_len, cur_len);
    }

 
    printf("\n");

    uchar* comm_buff = malloc(com_length);
    memcpy(comm_buff,   buff+read_length - com_length, com_length);
    printf("comments: %s\n", comm_buff);
    //buff += screen_buff_length;
    
}

int main(int argc, char const *argv[]){
	int err;
    
    patch();

	path = argv[1];
	
	printf("\n");
    printf("frm file: %s\n\n\n", path);

    frm = fopen(path, "rb");
    
    err = fread(frm_header_buff, sizeof(frm_header_buff), 1, frm);
    if(err == 0) {
    	printf("read header error \n");
    	exit(0);
    }

    printf("-----------------------------FRM header-----------------------------\n");
    show_header(frm, frm_header_buff);
    printf("-----------------------------key buff------------------------------\n");
    show_keybuff(frm, keybuff_file_pos, key_info_length);

    printf("-----------------------------forminfo------------------------------\n");
    show_forminfo(frm);

    printf("------------------------misc------------------------------\n");
    show_misc(frm, keybuff_file_pos, key_buff_length, reclength);

    printf("------------------------record defaults------------------------------\n");
    show_defaults(frm);
    
    printf("------------------------fields info------------------------------\n");
    show_fields_info(frm, 
    	             forminfo_pos, 
    	             create_fields__elements,
    	             fixed_field_pack_length,
    	             screen_buff_length,
                     n_length,
                     int_length,
                     com_length
    	             );
    //get_forminfo_pos(frm);

    fclose(frm);
	return 0;
}

