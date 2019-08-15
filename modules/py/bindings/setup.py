import setuptools

setuptools.setup(
    name="PyQNLPSimulator",
    version="0.0.1",
    author="Lee J. O\'Riordan, Myles Doyle",
    author_email="lee.oriordan@ichec.ie, myles.doyle@ichec.ie",
    description="Quantum NLP package",
    long_description="Quantum NLP project @ ICHEC",
    url="https://git.ichec.ie/intel-qnlp/intel-qnlp",
    packages=setuptools.find_packages(),
    package_data={'': ['_PyQNLPSimulator.*.so'],},
    classifiers=[
        "Programming Language :: Python :: 3",
    ],
)
