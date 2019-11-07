from abc import ABC, abstractmethod #Abstract base class

class EncoderBase(ABC):
    """Base class for binary encoding bitstring data"""
    def __init__(self):
        super().__init__()
    
    @abstractmethod
    def encode(self, bin_val):
        pass

    @abstractmethod
    def decode(self, bin_val):
        pass