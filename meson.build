project('my-cpp-project', 'cpp',
    version: '1.0.0',
    default_options: [
        'cpp_std=c++20',
        'warning_level=3',
        'werror=true', 
        'optimization=s' 
    ]
)

cpp_args = [
    '-Wpedantic',
    '-Wformat=2',
    '-Wundef',
    '-Wcast-align',
    '-Wwrite-strings',
    '-Wold-style-cast',
    '-Wnull-dereference',
    '-Wctor-dtor-privacy',
    '-Wno-missing-field-initializers',
]

add_global_arguments(cpp_args, language : 'cpp')

app_sources = [
    'main.cpp',
    'bank_customer.cpp',
    'bank.cpp', 
    'transaction.cpp',
]

executable('my_app',
    app_sources,
    install: true
)
