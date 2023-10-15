# The structure of koopa IR:


    Program - "koopa_raw_program_t"
        
        Global Variable List: - "koopa_raw_slice_t"
            
            Value 1
            VAlue 2
            ...
        
        Function List:
            
            Function 1
                
                Basic Block List:
                    
                    Basic Block 1
                        
                        Instruction List:
                            
                            Value 1
                            Value 2
                            ...
                    
                    Basic Block 2
                        ...
            
            Function 2
                ...


# The counterpart in "koopa.h" to each component in koopa IR

    1.koopa_raw_program_t
        koopa_raw_slice_t values;           -Variable List
        koopa_raw_slice_t funcs;            -Function List

    2. koopa_raw_slice_t
        const void **buffer                 -Block List
        uint32_t len                        -Length
        koopa_raw_slice_item_kind_t         -Kind

    3. koopa_raw_slice_item_kind            -Kind Enum
        KOOPA_RSIK_UNKNOWN                  -Unknown
        KOOPA_RSIK_TYPE                     -Type
        KOOPA_RSIK_FUNCTION                 -Function
        KOOPA_RSIK_BASIC_BLOCK              -Basic Block
        KOOPA_RSIK_VALUE                    -Value 

    4. koopa_raw_basic_block_data_t         
        const char *name                    -Name
        koopa_raw_slice_t params            -Parameters
        koopa_raw_slice_t used_by           -Values Of Block
        koopa_raw_slice_t insts             -Instructions


# An issue about stdout

## when using freopen to redirect output to a file, it's important to ensure that stdout is fully redirected before any further code writes to it. Otherwise, the output may not be correctly redirected to the desired file. To address this, you can use fflush(stdout) as a synchronization point to flush any buffered data to the file before proceeding with further code execution.
```cpp
freopen(output, "w", stdout);
fflush(stdout);
```
